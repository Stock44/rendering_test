//
// Created by hiram on 5/4/22.
//

#include "Mesh.h"

#include <utility>

namespace graphics {

    const std::vector<Vertex> &Mesh::getVertices() const {
        return vertices;
    }

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices) : vertices(std::move(vertices)),
                                                                          indices(std::move(indices)) {}


    const std::vector<uint> &Mesh::getIndices() const {
        return indices;
    }
} // graphics