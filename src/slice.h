#ifndef __TP_SLICE_H__
#define __TP_SLICE_H__

#include <Eigen/Dense>
#include <vector>

#include "tooldef.h"
#include "mesh.h"

using namespace std;
using namespace Eigen;

class lineseg {
    public:
        lineseg();
        lineseg(const lineseg &other);

        friend ostream& operator<< (ostream &out, const lineseg &l);

        Vector3f p1;
        Vector3f p2;
};

class levelset {
    public:
        levelset();
        levelset(const levelset &other);

        friend ostream& operator<< (ostream &out, const levelset &ls);

        // verteces on the levelset polygon
        vector<vertex*> perimeter;
        // edges of the levelset polygon. note that, since this is a polygon with
        // no adjacent polygons, none of the pair pointers on any of these edges
        // will be initialized, and are probably garbage.
        vector<edge*> edges;
        // faces that are entirely in the plane of this levelset
        vector<face*> inplane;
        // the height of this levelset
        float z;

        // faces in the original mesh that contribute to this levelset. this is
        // mostly for debug drawing.
        vector<face*> faces;
        // line segments in levelset polyline.
        vector<lineseg> lines;
};

void slice(tooldef td, mesh &m, vector<levelset> &out);

#endif
