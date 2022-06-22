//
// Created by hiram on 6/6/22.
//

#ifndef CITYY_MESH_H
#define CITYY_MESH_H

#include <vector>
#include "Vertex.h"

namespace graphics {
    struct Mesh {
        long ID;
        std::vector <Vertex> vertices;
        std::vector <GLuint> indices;
    };
}

#endif //CITYY_MESH_H
