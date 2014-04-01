#include "slice.h"

#include <iostream>

typedef struct {
    Vector3f p1;
    Vector3f p2;
} line;

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

void find_line_segments(levelset &ls, vector<line> &lines, vector<face*> inplane) {
    for (auto iter = ls.faces.begin(); iter != ls.faces.end(); iter++) {
        face* f = *iter;
        if (f->sides() != 3) {
            // TODO: triangularize!
            cout << "got a nontriangular mesh and I'm a stupid slicer." << endl;
            return;
        }

        vertex *v0 = f->e->vert;
        // check for whole-tri-in-plane condition
        bool is_inplane = true;
        do {
            if (v0->loc[2] != ls.z) {
                is_inplane = false;
                break;
            }
            v0 = v0->e->next->vert;
        } while (v0 != f->e->vert);
        if (is_inplane) {
            inplane.push_back(f);
            continue;
        }

        line l;
        int points_found = 0;
        v0 = f->e->vert;
        do {
            vertex *v = v0->e->next->vert;
            float v_z = v->loc[2],
                  v0_z = v0->loc[2];

            if (v_z == v0_z) {
                // check for edge-in-plane condition. since we know the whole
                // tri /isn't/ in-plane from the check above, we know that this
                // edge must be our levelset poly line segment
                if (v_z == ls.z) {
                    l.p1 = v0->loc;
                    l.p2 = v->loc;
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

            v0 = v;
        } while (v0 != f->e->vert);

        if (points_found != 2) {
            cout << "warning: face doesn't intersect z = " << ls.z << endl;
        }
    }
}

vector<levelset> slice(tooldef td, mesh &m) {
    bounds b = m.get_bounds();
    int level_count = ceil((b.max_z - b.min_z) / td.z_accuracy);
    vector<levelset> levelsets;
    for (int i = 0; i < level_count; i++) {
        levelset l;
        l.z = b.min_z + i * td.z_accuracy;
        levelsets.push_back(l);
    }
    levelset l;
    l.z = b.max_z;
    levelsets.push_back(l);

    bucket_faces(m, b, td.z_accuracy, levelsets);
    return levelsets;
}
