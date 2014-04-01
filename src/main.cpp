#include <fstream>
#include <iostream>

#include "draw.h"
#include "mesh.h"
#include "objparse.h"
#include "slice.h"
#include "tooldef.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [obj file]" << std::endl;
    }

    mesh m;
    ifstream in(argv[1]);
    load_obj(in, m);
    in.close();

    tooldef td;
    td.r = .2;
    td.z_accuracy = .5;
    auto levelsets = slice(td, m);

    start_draw(argc, argv, m, levelsets);
}
