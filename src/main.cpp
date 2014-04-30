#include <fstream>
#include <iostream>
#include <string.h>

#include <meshparse/mesh.h>

#include "draw.h"
#include "path.h"
#include "slice.h"
#include "tooldef.h"

using namespace meshparse;

using std::ifstream;
using std::cout;
using std::endl;
using std::vector;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " [obj file]" << endl;
        return 1;
    }

    mesh m;
    ifstream in(argv[1]);
    if (!load_mesh(argv[1], in, m)) {
        cout << "Mesh loader couldn't read file." << endl;
        in.close();
        return 3;
    }
    in.close();

    tooldef td;
    td.r = .2;
    td.z_accuracy = .5;

    vector<levelset> levelsets;
    slice(td, m, levelsets);
    cout << "finished slicing, got " << levelsets.size() << " levelsets" << endl;

    path p = generate_toolpath(levelsets, td);

    start_draw(argc, argv, m, levelsets, p);
}
