#include <fstream>
#include <iostream>
#include <string.h>

#include "draw.h"
#include "mesh.h"
#include "objparse.h"
#include "slice.h"
#include "stlparse.h"
#include "tooldef.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [obj file]" << std::endl;
        return 1;
    }

    mesh m;
    ifstream in(argv[1]);
    const char *ext = &argv[1][strlen(argv[1]) - 3];
    if (strncmp(ext, "stl", 3) == 0) {
        load_stl(in, m);
    } else if (strncmp(ext, "obj", 3) == 0) {
        load_obj(in, m);
    } else {
        std::cout << "don't know how to handle extension " << ext << std::endl;
        in.close();
        return 2;
    }
    in.close();

    tooldef td;
    td.r = .2;
    td.z_accuracy = .5;

    vector<levelset> levelsets;
    slice(td, m, levelsets);
    cout << "finished slicing, got " << levelsets.size() << " levelsets" << endl;

    start_draw(argc, argv, m, levelsets);
}
