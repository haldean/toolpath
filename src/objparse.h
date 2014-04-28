#ifndef __OBJPARSE_H__
#define __OBJPARSE_H__

#include <istream>

#include "mesh.h"

bool load_obj(istream& input, mesh& mesh);

#endif
