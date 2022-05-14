//
// Created by hiram on 5/4/22.
//

#include "Model.h"

namespace graphics {
    Model::Model(std::string name, std::vector<Vertex> vertices) : name(std::move(name)),
                                                                   vertices(std::move(vertices)) {}

    const std::vector<Vertex> &Model::getVertices() const {
        return vertices;
    }

    const std::string &Model::getName() const {
        return name;
    }

    Model::Model(std::string name, std::vector<Vertex> vertices, std::vector<uint> indices) : name(std::move(name)),
                                                                                              vertices(std::move(
                                                                                                      vertices)),
                                                                                              indices({indices}) {}

    bool Model::usesElements() const {
        return indices.has_value();
    }

    const std::vector<uint> &Model::getIndices() const {
        if (!usesElements()) throw std::bad_optional_access();
        return indices.value();
    }
} // graphics