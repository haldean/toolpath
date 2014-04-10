#include "objparse.h"

#include <cstdio>
#include <iterator>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

void load_stl(istream& file, mesh &mesh) {
    string line;
    while (file.good()) {
        getline(file, line);
    }

    cout << "Loaded mesh: " << endl
        << "  " << mesh.verteces.size() << " verteces." << endl
        << "  " << mesh.edges.size() << " edges." << endl;
}

