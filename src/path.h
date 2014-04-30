#ifndef __TP_PATH_H__
#define __TP_PATH_H__

#include <vector>
#include <Eigen/Dense>

#include "slice.h"
#include "tooldef.h"

using namespace Eigen;

class path {
    public:
        std::vector<Vector3f> points;
};

path generate_toolpath(const std::vector<levelset> &levelsets, const tooldef);

#endif
