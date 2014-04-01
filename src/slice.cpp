#include "slice.h"

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
