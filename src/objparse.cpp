#include "objparse.h"

#include <cstdio>
#include <iterator>
#include <vector>
#include <iostream>

#include <Eigen/Dense>

#include "parse_common.h"

using namespace Eigen;
using std::string;
using std::vector;

struct objface {
    unsigned int id;
    vector<unsigned int> vids;
    vector<unsigned int> nids;
};

vector<struct objface> objfaces;

int obj_parse_vertex_spec(const string vspec) {
    unsigned int vid;
    sscanf(vspec.c_str(), "%d", &vid);
    return vid;
}

void obj_parse_line(mesh &objm, string line) {
    // TODO: support vn directives
    if (line[0] == 'v' && line[1] == ' ') {
        vertex *v = new vertex();

        float x, y, z;
        sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
        v->loc = Vector3f(x, y, z);
        v->id = objm.verteces.size() + 1;

        objm.verteces.push_back(v);
    } else if (line[0] == 'f' && line[1] == ' ') {
        std::istringstream tokenizer(line);
        vector<string> tokens;
        copy(std::istream_iterator<string>(tokenizer),
                std::istream_iterator<string>(),
                std::back_inserter<vector<string> >(tokens));

        struct objface f;
        f.id = objfaces.size() + 1;
        for (unsigned int i=1; i<tokens.size(); i++) {
            string token = tokens[i];
            f.vids.push_back(obj_parse_vertex_spec(token));
        }

        objfaces.push_back(f);
    }
}

void obj_add_triangle(mesh &objm, vector<unsigned int> vids) {
    face *f = new face();
    f->id = objm.faces.size() + 1;

    edge *last_edge;
    for (unsigned int i = 0; i < vids.size(); i++) {
        edge *e = new edge();
        e->vert = objm.verteces[vids[i] - 1];
        objm.verteces[vids[i] - 1]->e = e;
        e->f = f;
        e->id = objm.edges.size() + 1;
        e->pair = NULL;
        if (i != 0) {
            last_edge->next = e;
        } else {
            f->e = e;
        }

        last_edge = e;
        objm.edges.push_back(e);
    }
    last_edge->next = f->e;
    f->calculate_normal();
    objm.faces.push_back(f);
}

bool load_obj(std::istream& file, mesh &mesh) {
    if (!file.good()) {
        std::cout << "Could not read file." << std::endl;
        return false;
    }

    string line;
    while (file.good()) {
        getline(file, line);
        obj_parse_line(mesh, line);
    }

    for (auto objf = objfaces.begin(); objf != objfaces.end(); objf++) {
        obj_add_triangle(mesh, objf->vids);
    }
    merge_half_edges(mesh);

    std::cout << "Loaded mesh: " << std::endl
        << "  " << mesh.verteces.size() << " verteces." << std::endl
        << "  " << mesh.edges.size() << " edges." << std::endl
        << "  " << objfaces.size() << " faces in OBJ file." << std::endl;
    return true;
}

