#include "mesh.h"
#include "slice.h"

face* mktri(
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3) {
    vertex *v1 = new vertex(),
           *v2 = new vertex(),
           *v3 = new vertex();
    v1->loc = Vector3f(x1, y1, z1);
    v2->loc = Vector3f(x2, y2, z2);
    v3->loc = Vector3f(x3, y3, z3);

    edge *e1 = new edge(),
         *e2 = new edge(),
         *e3 = new edge();

    e1->vert = v1;
    v1->e = e1;
    e1->next = e2;
    e1->pair = NULL;

    e2->vert = v2;
    v2->e = e2;
    e2->next = e3;
    e2->pair = NULL;

    e3->vert = v3;
    v3->e = e3;
    e3->next = e1;
    e3->pair = NULL;

    face *f = new face();
    f->e = e1;
    e1->f = f;
    e2->f = f;
    e3->f = f;

    return f;
}

int main(int argc, char* argv[]) {
    face *tri = mktri(
            9.807850, -1.950900, 0.000000,
            9.807850, -1.950900, -5.000000,
            9.871900, -1.300600, -6.666667);
    lineseg l = isect_tri_xy_plane(-5, tri);
    std::cout << "found lineseg:" << std::endl << l << std::endl;
    std::cout << "length: " << (l.p1 - l.p2).norm() << std::endl;
}
