#include "draw.h"

#include <sstream>

#include "drawmesh.h"
#include "glinclude.h"

mesh *global_mesh;
drawopts opts;

GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat position[] = {0.0, 0.0, 20.0, 1.0};
GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};

GLfloat camRotX = 0;
GLfloat camRotY = 0;

void change_size(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the clipping volume
    gluPerspective(45.0f, (GLfloat) w / (GLfloat) h, .001f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void special_key(unsigned char key, int x, int y) {
    if (key == 'w') {
        camRotX += 10.;
    } else if (key == 's') {
        camRotX -= 10.;
    } else if (key == 'a') {
        camRotY += 10.;
    } else if (key == 'd') {
        camRotY -= 10.;
    }

    glutPostRedisplay();
}

void on_draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();	{
        glTranslatef(0, 0, -40.f);
        glRotatef(camRotX, 1, 0, 0);
        glRotatef(camRotY, 0, 1, 0);

        draw_mesh(*global_mesh, opts);
    } glPopMatrix();

    ostringstream info;
    info << global_mesh->faces.size() << " faces";
    draw_string(info.str());

    glFlush();
}

void start_draw(int argc, char *argv[], mesh *m) {
    global_mesh = m;

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
    glutMainLoop();
}
