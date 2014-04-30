#include "path.h"

using std::vector;

path generate_toolpath(const vector<levelset> &levelsets, const tooldef td) {
    path p;
    for (auto ls = levelsets.begin(); ls != levelsets.end(); ls++) {
        auto perims = ls->perimeters;
        for (auto perim = perims.begin(); perim != perims.end(); perim++) {
            for (auto v = perim->begin(); v != perim->end(); v++) {
                p.points.push_back(Vector3f(ls->verteces[*v]));
            }
        }
    }
    return p;
}
