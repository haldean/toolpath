#ifndef __TP_SLICE_H__
#define __TP_SLICE_H__

#include <Eigen/Dense>
#include <vector>

#include "tooldef.h"
#include "mesh.h"

using namespace std;
using namespace Eigen;

typedef struct {
    // verteces on the levelset polygon
    vector<vertex*> perimeter;
    // edges of the levelset polygon. note that, since this is a polygon with
    // no adjacent polygons, none of the pair pointers on any of these edges
    // will be initialized, and are probably garbage.
    vector<edge*> edges;
    // the height of this levelset
    float z;

    // faces in the original mesh that contribute to this levelset. this is
    // mostly for debug drawing.
    vector<face*> faces;
} levelset;

vector<levelset> slice(tooldef td, mesh &m);

#endif
