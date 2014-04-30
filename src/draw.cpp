#include "draw.h"

#include <sstream>
#include <vector>

#include "drawmesh.h"
#include "glinclude.h"

#define ONLY_MESH 0x01
#define ONLY_PATH 0x02
#define MESH_AND_PATH (ONLY_MESH | ONLY_PATH)

using namespace meshparse;

using std::cout;
using std::endl;
using std::ostringstream;
using std::vector;

mesh global_mesh;
path global_path;
vector<levelset> levelsets;
drawopts opts;
bounds mesh_bounds;

GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat position[] = {0.0, 0.0, 20.0, 1.0};
GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};

int layer = -1;
int showPath = ONLY_MESH;

int lastX = -1;
int lastY = -1;

GLfloat camDistance = 40.f;
GLfloat rotAroundZ;
GLfloat rotAroundY;
GLfloat rotSpeed = .01;

void change_size(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the clipping volume
    gluPerspective(45.0f, (GLfloat) w / (GLfloat) h, .001f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void on_press(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        lastX = x;
        lastY = y;
    } else {
        lastX = -1;
        lastY = -1;
    }
}

void on_motion(int x, int y) {
    rotAroundZ += rotSpeed * (x - lastX);
    rotAroundY += rotSpeed * (y - lastY);
    glutPostRedisplay();
}

void special_key(unsigned char key, int x, int y) {
    if (key == 'w') {
        rotAroundY += 10.;
    } else if (key == 's') {
        rotAroundY -= 10.;
    } else if (key == 'a') {
        rotAroundZ += 10.;
    } else if (key == 'd') {
        rotAroundZ -= 10.;
    } else if (key == 'q') {
        camDistance += 2.;
    } else if (key == 'e') {
        camDistance -= 2.;
    } else if (key == 'n') {
        layer++;
        if (layer == levelsets.size()) {
            layer = -1;
        }
    } else if (key == 'p') {
        layer--;
        if (layer == -2) {
            layer = levelsets.size() - 1;
        }
    } else if (key == 't') {
        showPath = showPath + 1;
        if (showPath == 4) {
            showPath = 1;
        }
    }

    glutPostRedisplay();
}

void on_draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();	{
        gluLookAt(camDistance, 0, 0, 0, 0, 0, 0, 0, 1);
        glRotatef(rotAroundZ, 0, 0, 1);
        glRotatef(rotAroundY, 0, 1, 0);

        if (showPath & ONLY_PATH) {
            draw_path(global_path, opts);
        }
        if (showPath & ONLY_MESH) {
            if (layer == -1) {
                draw_mesh(global_mesh, opts);
            } else {
                levelset ls = levelsets[layer];
                if (ls.perimeters.size() > 0) {
                    draw_perimeters(ls.verteces, ls.perimeters, opts);
                } else if (ls.lines.size() > 0) {
                    draw_linesegs(ls.lines, opts);
                } else {
                    draw_faces(ls.faces, opts);
                    draw_xy_plane(ls.z, mesh_bounds, opts);
                }
            }
        }
    } glPopMatrix();

    ostringstream info;
    info << global_mesh.faces.size() << " faces, ";
    info << levelsets.size() << " layers, ";
    info << "showing ";
    if (layer == -1) {
        info << "all layers";
    } else {
        info << "layer " << layer;
        info << ", z = " << levelsets[layer].z;
    }
    draw_string(info.str());

    glFlush();
}

void start_draw(int argc, char *argv[], mesh &m, vector<levelset> &ls, path &p) {
    global_mesh = m;
    global_path = p;
    levelsets = ls;
    mesh_bounds = m.get_bounds();

    opts = default_draw_options();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("toolpath");

    glClearColor(.7, .7, .7, 1.);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    glutDisplayFunc(on_draw);
    glutReshapeFunc(change_size);
    glutKeyboardFunc(special_key);
    glutMouseFunc(on_press);
    glutMotionFunc(on_motion);
    glutMainLoop();
}
