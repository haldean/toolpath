#include "slice.h"

#include <cstdio>
#include <iostream>
#include <map>
#include <stdint.h>

#include <Eigen/Sparse>

#define OUT_OF_PLANE (0)
#define POINT_IN_PLANE (1)
#define FACE_IN_PLANE (2)

// returns FACE_IN_PLANE if the face is fully in-plane (all verteces lie in the
// plane). returns POINT_IN_PLANE if the plane and face intersect at a single
// point. returns OUT_OF_PLANE if neither property holds.
//
// this function should only be called on faces that either intersect the plane
// or lie in it.
int inplane_status(const float z, const face* f) {
    edge *e0 = f->e;
    // check for in-plane conditions
    int verts_inplane = 0;
    int verts_above = 0;
    int verts_below = 0;

    do {
        vertex *v = e0->vert;
        if (v->loc[2] == z) {
            verts_inplane++;
        } else if (v->loc[2] > z) {
            verts_above++;
        } else {
            verts_below++;
        }
        e0 = e0->next;
    } while (e0 != f->e);

    // check for whole-tri-in-plane
    if (verts_above == 0 && verts_below == 0) {
        return FACE_IN_PLANE;

    }
    // check for single-point-in-plane
    if (verts_inplane == 1 && (verts_above == 0 || verts_below == 0)) {
        return POINT_IN_PLANE;
    }
    return OUT_OF_PLANE;
}

// returns the line segment across the face representing the line of
// intersection between the xy-plane at height z and the provided face.
// returns a zero-length line segment if face does not intersect plane
lineseg isect_tri_xy_plane(const float z, const face* f) {
    lineseg l;

    if (f->sides() != 3) {
        // TODO: triangularize!
        cout << "got a nontriangular mesh and I'm a stupid slicer." << endl;
        return l;
    }

    int points_found = 0;
    edge *e0 = f->e;
    do {
        // do this at the start so we can use continue within the loop
        e0 = e0->next;

        vertex *v = e0->next->vert,
               *v0 = e0->vert;
        float v_z = v->loc[2],
              v0_z = v0->loc[2];

        if (v_z == v0_z) {
            // check for edge-in-plane condition. since we know the whole
            // tri /isn't/ in-plane from the check above, we know that this
            // edge must be our levelset poly line segment
            if (v_z == z) {
                l.p1 = v0->loc;
                l.p2 = v->loc;
                points_found = 2;
                break;
            }
            continue;
        }

        float n = (v_z - z) / (v_z - v0_z);
        if (0 <= n && n <= 1) {
            Vector3f p = v->loc - n * (v->loc - v0->loc);
            if (points_found == 0) {
                l.p1 = p;
                points_found++;
            } else if (points_found == 1 && p != l.p1) {
                l.p2 = p;
                points_found++;
            }
        }
    } while (e0 != f->e);
    return l;
}

// assign faces to layer buckets. this modifies the levelset vector in-place.
void bucket_faces(
        const mesh &m, const bounds &b,
        const float layer_height, vector<levelset> &layers) {
    for (auto iter = m.faces.begin(); iter != m.faces.end(); iter++) {
        face* f = *iter;

        // find face bounds
        float z_min = INFINITY, z_max = -INFINITY;
        edge *e = f->e;
        vertex *v;
        do {
            v = e->vert;
            if (v->loc[2] > z_max) {
                z_max = v->loc[2];
            }
            if (v->loc[2] < z_min) {
                z_min = v->loc[2];
            }
            e = e->next;
        } while (e != f->e);

        // add face to appropriate buckets
        for (auto ls = layers.begin(); ls != layers.end(); ls++) {
            if (z_min <= ls->z && ls->z <= z_max) {
                ls->faces.push_back(f);
            } else if (ls->z > z_max) {
                // we're out of our good range, break
                break;
            }
        }
    }
}

// generates a list of line segments based on the intersection of the bucketed
// faces and the xy-plane at height ls.z
void find_line_segments(levelset &ls) {
    for (auto iter = ls.faces.begin(); iter != ls.faces.end(); iter++) {
        face* f = *iter;
        int inplane = inplane_status(ls.z, f);

        if (inplane == FACE_IN_PLANE) {
            ls.inplane.push_back(f);
            continue;
        } else if (inplane == POINT_IN_PLANE) {
            continue;
        }

        lineseg line = isect_tri_xy_plane(ls.z, f);
        if ((line.p1 - line.p2).norm() == 0) {
            cout << "warning: face doesn't intersect z = " << ls.z << endl;
            edge *e = f->e;
            do {
                vertex *v = e->vert;
                printf("  v[%lx]\t%f\t%f\t%f\n", (unsigned long) v,
                        v->loc[0], v->loc[1], v->loc[2]);
                e = e->next;
            } while (e != f->e);
        } else {
            ls.lines.push_back(line);
        }
    }
}

// creates a vertex in the vert_ids map and in the verts list. returns the ID to
// use for the next vertex (either next_id or next_id + 1)
int create_vertex(
        map<Vector3f, uint32_t> &vert_ids, Vector3f &loc,
        vector<Vector3f> &verts, int next_id) {
    if (vert_ids.find(loc) != vert_ids.end()) {
        return next_id;
    }
    vert_ids.insert(pair<Vector3f, int>(loc, next_id));
    verts.push_back(loc);
    return next_id + 1;
}

// converts an unsorted list of line segments to a list of ordered lists of
// verteces representing paths around the levelset.
void linesegs_to_vert_list(levelset &ls) {
    map<Vector3f, uint32_t> vert_ids;
    vector<Vector3f> verts;
    int next_id = 0;

    // build vert list
    for (auto iter = ls.lines.begin(); iter != ls.lines.end(); iter++) {
        next_id = create_vertex(vert_ids, iter->p1, verts, next_id);
        next_id = create_vertex(vert_ids, iter->p2, verts, next_id);
    }
    int n = verts.size();

    // build adjacency
    vector<Triplet<uint8_t>> adjacents;
    for (auto iter = ls.lines.begin(); iter != ls.lines.end(); iter++) {
        auto i1 = vert_ids.find(iter->p1);
        if (i1 == vert_ids.end()) {
            cout << "warning: after building vert list, vert " <<
                iter->p1.transpose() << " is missing" << endl;
        }
        auto i2 = vert_ids.find(iter->p2);
        if (i2 == vert_ids.end()) {
            cout << "warning: after building vert list, vert " <<
                iter->p2.transpose() << " is missing" << endl;
        }
        adjacents.push_back(*i1, *i2, 1);
    }

    SparseMatrix<uint8_t> adj(n, n);
    adj.setFromTriplets(adjacents.start(), adjacents.end());
}

void slice(const tooldef td, const mesh &m, vector<levelset> &levelsets) {
    levelsets.clear();

    bounds b = m.get_bounds();
    int level_count = ceil((b.max_z - b.min_z) / td.z_accuracy);
    for (int i = 0; i < level_count; i++) {
        levelset l;
        l.z = b.min_z + i * td.z_accuracy;
        levelsets.push_back(l);
    }
    levelset l;
    l.z = b.max_z;
    levelsets.push_back(l);

    bucket_faces(m, b, td.z_accuracy, levelsets);

    for (auto iter = levelsets.begin(); iter != levelsets.end(); iter++) {
        find_line_segments(*iter);
        linesegs_to_vert_list(*iter);
    }
}

lineseg::lineseg() {}

lineseg::lineseg(const lineseg &other) : p1(other.p1), p2(other.p2) {}

levelset::levelset() {}

levelset::levelset(const levelset &other) :
        perimeter(other.perimeter), edges(other.edges), inplane(other.inplane),
        z(other.z), faces(other.faces), lines(other.lines) {
}

ostream& operator<< (ostream &out, const lineseg &l) {
    out << "(" << l.p1.transpose() << ",\t" << l.p2.transpose() << ")";
    return out;
}

ostream& operator<< (ostream &out, const levelset &ls) {
    out << "[perimeter size = " << ls.perimeter.size()
        << " lines size " << ls.lines.size()
        << "]";
    return out;
}
