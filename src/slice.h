#ifndef __TP_SLICE_H__
#define __TP_SLICE_H__

#include <Eigen/Dense>
#include <vector>

#include "tooldef.h"
#include "mesh.h"

using namespace Eigen;

class lineseg {
    public:
        lineseg();
        lineseg(const lineseg &other);

        friend std::ostream& operator<< (std::ostream &out, const lineseg &l);

        Vector3f p1;
        Vector3f p2;
};

class levelset {
    public:
        levelset();
        levelset(const levelset &other);

        friend std::ostream& operator<< (std::ostream &out, const levelset &ls);

        // verteces on the levelset polygon
        std::vector<Vector3f> verteces;
        // list of connected perimeters for the levelset. values are indeces
        // into the perimeter array
        std::vector<std::vector<uint32_t>> perimeters;
        // faces that are entirely in the plane of this levelset
        std::vector<face*> inplane;
        // the height of this levelset
        float z;

        // faces in the original mesh that contribute to this levelset. this is
        // mostly for debug drawing.
        std::vector<face*> faces;
        // line segments in levelset polyline.
        std::vector<lineseg> lines;
};

int inplane_status(const float z, const face* f);
lineseg isect_tri_xy_plane(const float z, const face* f);
void bucket_faces(
        const mesh &m, const bounds &b, const float layer_height,
        const std::vector<levelset>&);
void find_line_segments(levelset &ls);
void slice(const tooldef td, const mesh &m, std::vector<levelset> &out);

#endif
