#include <fstream>
#include <iostream>

#include "draw.h"
#include "mesh.h"
#include "objparse.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [obj file]" << std::endl;
    }

    mesh m;
    ifstream in(argv[1]);
    load_obj(in, m);
    in.close();

    start_draw(argc, argv, &m);
}
