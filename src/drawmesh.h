#ifndef __TP_DRAWMESH_H__
#define __TP_DRAWMESH_H__

#include <meshparse/mesh.h>
#include <vector>

#include "glinclude.h"
#include "slice.h"

using namespace meshparse;

struct draw_options_struct {
  bool draw_edges;
  bool draw_normals;
  bool draw_faces;
  bool draw_verteces;
  bool draw_hull;
  GLfloat edge_color[4];
  GLfloat mesh_color[4];
  GLfloat normal_color[4];
  GLfloat hull_color[4];
} typedef drawopts;

drawopts default_draw_options();
void draw_hull(mesh&, drawopts);
void draw_mesh(mesh&, drawopts);
void draw_string(std::string);
void draw_xy_plane(float z, bounds&, drawopts);
void draw_faces(std::vector<face*>, drawopts);
void draw_linesegs(std::vector<lineseg>, drawopts);
void draw_perimeters(std::vector<Vector3f>, std::vector<std::vector<uint32_t>>, drawopts);

#endif
