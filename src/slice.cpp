#include "slice.h"

#include <cstdio>
#include <iostream>

// assign faces to layer buckets. this modifies the levelset vector in-place.
void bucket_faces(mesh &m, bounds &b, float layer_height, vector<levelset> &layers) {
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

void find_line_segments(levelset &ls) {
    for (auto iter = ls.faces.begin(); iter != ls.faces.end(); iter++) {
        face* f = *iter;
        if (f->sides() != 3) {
            // TODO: triangularize!
            cout << "got a nontriangular mesh and I'm a stupid slicer." << endl;
            return;
        }

        edge *e0 = f->e;
        // check for in-plane conditions
        int verts_inplane = 0;
        int verts_above = 0;
        int verts_below = 0;
        do {
            vertex *v = e0->vert;
            if (v->loc[2] == ls.z) {
                verts_inplane++;
            } else if (v->loc[2] > ls.z) {
                verts_above++;
            } else {
                verts_below++;
            }
            e0 = e0->next;
        } while (e0 != f->e);

        // check for whole-tri-in-plane
        if (verts_inplane == 3) {
            ls.inplane.push_back(f);
            continue;
        }
        // check for single-point-in-plane
        if (verts_inplane == 1 && (verts_above == 0 || verts_below == 0)) {
            // we don't need the zero-length line segment represented by this
            // point to create this level set. just skip it.
            continue;
        }

        lineseg l;
        int points_found = 0;
        e0 = f->e;
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
                if (v_z == ls.z) {
                    l.p1 = v0->loc;
                    l.p2 = v->loc;
                    points_found = 2;
                    break;
                }
                continue;
            }

            float n = (ls.z) / (v_z - v0_z);
            if (0 <= n && n <= 1) {
                Vector3f p = v0->loc + n * (v->loc - v0->loc);
                if (++points_found == 1) {
                    l.p1 = p;
                } else {
                    l.p2 = p;
                }
            }
        } while (e0 != f->e);

        if (points_found != 2) {
            cout << "warning: face doesn't intersect z = " << ls.z << endl;
            cout << "  found " << points_found << " point(s)" << endl;
            edge *e = f->e;
            do {
                vertex *v = e->vert;
                printf("  v[%lx]\t%f\t%f\t%f\n", (unsigned long) v,
                        v->loc[0], v->loc[1], v->loc[2]);
                e = e->next;
            } while (e != f->e);
        } else {
            ls.lines.push_back(l);
        }
    }
}

void slice(tooldef td, mesh &m, vector<levelset> &levelsets) {
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
    }
}

lineseg::lineseg() {}

lineseg::lineseg(const lineseg &other) : p1(other.p1), p2(other.p2) {}

levelset::levelset() {}

levelset::levelset(const levelset &other) :
        perimeter(other.perimeter), edges(other.edges), inplane(other.inplane),
        z(other.z), faces(other.faces), lines(other.lines) {
}

ostream& operator<< (ostream &out, const levelset &ls) {
    out << "[perimeter size = " << ls.perimeter.size()
        << " lines size " << ls.lines.size()
        << "]";
    return out;
}
