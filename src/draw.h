#ifndef __TP_DRAW_H__
#define __TP_DRAW_H__

#include <vector>
#include <meshparse/mesh.h>

#include "slice.h"

using namespace meshparse;

void start_draw(int argc, char *argv[], mesh&, std::vector<levelset>&);

#endif
