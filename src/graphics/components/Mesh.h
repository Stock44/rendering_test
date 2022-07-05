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
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Mesh(long ID, std::vector<Vertex> vertices, std::vector<unsigned int> indices) : ID(ID), vertices(
                std::move(vertices)), indices(std::move(indices)) {}

        Mesh(Mesh const &other) = delete;

        Mesh operator=(Mesh const &other) = delete;
    };
}

#endif //CITYY_MESH_H
