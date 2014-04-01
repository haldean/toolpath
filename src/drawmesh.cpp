#include <Eigen/Core>
#include <iostream>
#include <cstring>

#include "drawmesh.h"
#include "glinclude.h"

void draw_string(string s) {
    glDisable(GL_LIGHTING);

    int matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */

    glRasterPos3f(0.01, 0.01, 0);
    glColor4f(1., 1., 1., 1.);
    for (unsigned int i = 0; i < s.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
    }

    glPopAttrib();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(matrixMode);

    glEnable(GL_LIGHTING);
}

void vector3_to_gl(Vector3f v) {
    glVertex3f(v[0], v[1], v[2]);
}

void draw_vertex(vertex &v, face *face, drawopts opts) {
    if (!v.normal.isZero()) {
        glNormal3f(v.normal[0], v.normal[1], v.normal[2]);
    } else {
        glNormal3f(face->normal[0], face->normal[1], face->normal[2]);
    }
    vector3_to_gl(v.loc);
}

void draw_face(face* face, drawopts opts) {
    if (opts.draw_faces) {
        if (face->sides() == 3) glBegin(GL_TRIANGLES);
        //else if (face->sides() == 4) glBegin(GL_QUADS);
        else glBegin(GL_TRIANGLE_FAN);

        edge *e0 = face->e;
        edge *e = e0;
        do {
            draw_vertex(*e->vert, face, opts);
            e = e->next;
        } while (e != e0);

        glEnd();
    }
}

void draw_edges(face* face, drawopts opts) {
    glBegin(GL_LINE_STRIP); {
        edge *e0 = face->e;
        edge *e = e0;
        do {
            vector3_to_gl(e->vert->loc);
            e = e->next;
        } while (e != e0);
        vector3_to_gl(e0->vert->loc);
    } glEnd();
}

void draw_normals(face* face, drawopts opts) {

    for (int i = 0; i < 3; i++) {
        edge *e0 = face->e;
        edge *e = e0;
        do {
            vertex *v = e->vert;
            glBegin(GL_LINE_STRIP); {
                vector3_to_gl(v->loc);
                vector3_to_gl(v->loc + v->normal);
            } glEnd();
            e = e->next;
        } while (e != e0);
    }
}

void draw_hull(mesh &mesh, drawopts opts) {
    if (!opts.draw_hull) return;

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    glLineWidth(2.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.hull_color);

    for (auto fit = mesh.faces.begin(); fit != mesh.faces.end(); fit++) {
        draw_edges(*fit, opts);
    }

    glutWireCube(1.);
}

void draw_faces(vector<face*> faces, drawopts opts) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.mesh_color);
    for (auto it = faces.begin(); it != faces.end(); it++) {
        draw_face(*it, opts);
    }

    if (opts.draw_edges) {
        glLineWidth(2.0);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.edge_color);
        for (auto it = faces.begin(); it != faces.end(); it++) {
            draw_edges(*it, opts);
        }
    }
}

void draw_mesh(mesh &mesh, drawopts opts) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    if (opts.draw_faces || opts.draw_verteces) {
        draw_faces(mesh.faces, opts);
    }

    if (opts.draw_normals) {
        glLineWidth(1.0);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.normal_color);
        for (auto v = mesh.verteces.begin(); v != mesh.verteces.end(); v++) {
            glBegin(GL_LINE_STRIP); {
                vector3_to_gl((*v)->loc);
                vector3_to_gl((*v)->loc + (*v)->normal);
            } glEnd();
        }
    }

    if (opts.draw_verteces) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.mesh_color);
        for (auto v = mesh.verteces.begin(); v != mesh.verteces.end(); v++) {
            glPushMatrix(); {
                glTranslatef((*v)->loc[0], (*v)->loc[1], (*v)->loc[2]);
                glutSolidSphere(0.1, 5, 5);
            } glPopMatrix();
        }
    }
}

void draw_xy_plane(float z, bounds &b, drawopts opts) {
    glLineWidth(1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, opts.normal_color);

    float x_step = (b.max_x - b.min_x) / 20.;
    float y_step = (b.max_y - b.min_y) / 20.;

    float xp_lim = b.max_x + 2;
    float xn_lim = b.min_x - 2;
    float yp_lim = b.max_y + 2;
    float yn_lim = b.min_y - 2;

    for (float x = b.min_x; x <= b.max_x; x += x_step) {
        glBegin(GL_LINE_STRIP); {
            glVertex3f(x, yn_lim, z);
            glVertex3f(x, yp_lim, z);
        } glEnd();
    }
    for (float y = b.min_y; y <= b.max_y; y += y_step) {
        glBegin(GL_LINE_STRIP); {
            glVertex3f(xn_lim, y, z);
            glVertex3f(xp_lim, y, z);
        } glEnd();
    }
}

drawopts default_draw_options() {
    drawopts opts;
    opts.draw_edges = true;
    opts.draw_normals = false;
    opts.draw_faces = true;
    opts.draw_verteces = false;
    opts.draw_hull = false;

    opts.normal_color[0] = .0;
    opts.normal_color[1] = .0;
    opts.normal_color[2] = .0;
    opts.normal_color[3] = 1.;

    for (int i=0; i<3; i++) opts.edge_color[i] = 0.;
    opts.edge_color[3] = .2;

    opts.mesh_color[0] = 0.;
    opts.mesh_color[1] = .5;
    opts.mesh_color[2] = 1.;
    opts.mesh_color[3] = 1.;

    for (int i=0; i<3; i++) opts.hull_color[i] = 0.;
    opts.hull_color[3] = .3;

    return opts;
}
