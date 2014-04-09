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
        vector<Vector3f> verteces;
        // list of connected perimeters for the levelset. values are indeces
        // into the perimeter array
        vector<vector<uint32_t>> perimeters;
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

int inplane_status(const float z, const face* f);
lineseg isect_tri_xy_plane(const float z, const face* f);
void bucket_faces(
        const mesh &m, const bounds &b, const float layer_height,
        const vector<levelset>&);
void find_line_segments(levelset &ls);
void slice(const tooldef td, const mesh &m, vector<levelset> &out);

#endif
