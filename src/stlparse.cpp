#include "objparse.h"

#include <cstdio>
#include <iterator>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>
#include <re2/re2.h>

#define DONE 0
#define BAD_FILE 1
#define END_SOLID 2

using namespace Eigen;
using namespace re2;

// may god have mercy on my soul.
RE2 empty_re("^[\\s]*$");
RE2 endface_re("^[\\s]*endfacet[\\s]*$");
RE2 endloop_re("^[\\s]*endloop[\\s]*$");
RE2 endsolid_re("^[\\s]*endsolid[\\s]*([\\w]*)[\\s]*$");
RE2 facet_re("^[\\s]*facet[\\s]+normal[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]*$");
RE2 oloop_re("^[\\s]*outer loop[\\s]*$");
RE2 solid_re("^[\\s]*solid[\\s]*([\\w]*)[\\s]*$");
RE2 vertex_re("^[\\s]*vertex[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]+([\\d.e-]+)[\\s]*$");

typedef struct {
    unsigned int next_vertid;
    map<Vector3f, int> vert_to_id;
} stlmesh;

bool check_pattern_compilation() {
    bool ok = true;
    ok &= empty_re.ok();
    ok &= endface_re.ok();
    ok &= endloop_re.ok();
    ok &= endsolid_re.ok();
    ok &= facet_re.ok();
    ok &= oloop_re.ok();
    ok &= solid_re.ok();
    ok &= vertex_re.ok();
    return ok;
}

string next_nonempty_line(istream &file) {
    string line;
    while (file.good() && RE2::FullMatch(line, empty_re)) {
        getline(file, line);
    }
    return line;
}

int try_parse_face(istream& file) {
    string line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        std::cout << "File ended in middle of solid." << std::endl;
        return BAD_FILE;
    }
    if (RE2::FullMatch(line, endsolid_re)) {
        return END_SOLID;
    }

    string nxs, nys, nzs;
    float nx, ny, nz;
    if (RE2::FullMatch(line, facet_re, &nxs, &nys, &nzs)) {
        nx = std::stod(nxs);
        ny = std::stod(nys);
        nz = std::stod(nzs);
    } else {
        std::cout << "Expecting face, got " << line << std::endl;
        return BAD_FILE;
    }

    line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        std::cout << "File ended in middle of face." << std::endl;
        return BAD_FILE;
    }
    if (!RE2::FullMatch(line, oloop_re)) {
        std::cout << "Expecting outer loop, got " << line << std::endl;
        return BAD_FILE;
    }

    float verts[9];
    for (int i = 0; i < 3; i++) {
        line = next_nonempty_line(file);
        if (RE2::FullMatch(line, empty_re)) {
            std::cout << "File ended in middle of face." << std::endl;
            return BAD_FILE;
        }
        string vx, vy, vz;
        if (RE2::FullMatch(line, vertex_re, &vx, &vy, &vz)) {
            verts[i + 0] = std::stod(vx);
            verts[i + 1] = std::stod(vy);
            verts[i + 2] = std::stod(vz);
        } else {
            std::cout << "Expected vertex, got " << line << std::endl;
            return BAD_FILE;
        }
    }

    line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        std::cout << "File ended in middle of face." << std::endl;
        return BAD_FILE;
    }
    if (!RE2::FullMatch(line, endloop_re)) {
        std::cout << "Expecting outer loop end, got " << line << std::endl;
        return BAD_FILE;
    }

    line = next_nonempty_line(file);
    if (RE2::FullMatch(line, empty_re)) {
        std::cout << "File ended in middle of face." << std::endl;
        return BAD_FILE;
    }
    if (!RE2::FullMatch(line, endface_re)) {
        std::cout << "Expecting facet end, got " << line << std::endl;
        return BAD_FILE;
    }

    return DONE;
}

bool load_stl(istream& file, mesh &mesh) {
    if (!check_pattern_compilation()) {
        std::cout << "Regular expressions didn't compile, abort." << std::endl;
        return false;
    }

    if (!file.good()) {
        std::cout << "Could not read file." << std::endl;
        return false;
    }

    // find the start of the solid
    string line;
    string name;
    while (file.good()) {
        getline(file, line);
        if (RE2::FullMatch(line, solid_re, &name)) {
            std::cout << "Found solid " << name << std::endl;
            break;
        }
    }

    // parse the solid itself
    while (file.good()) {
        int res = try_parse_face(file);
        if (res == DONE) {
            continue;
        } else if (res == BAD_FILE) {
            std::cout << "Could not load mesh." << std::endl;
            return false;
        } else if (res == END_SOLID) {
            break;
        }
    }

    std::cout << "Loaded mesh: " << std::endl
        << "  " << mesh.verteces.size() << " verteces." << std::endl
        << "  " << mesh.edges.size() << " edges." << std::endl;
    return true;
}

