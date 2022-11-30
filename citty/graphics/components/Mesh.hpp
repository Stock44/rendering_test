//
// Created by hiram on 6/6/22.
//
#pragma once

#include <vector>
#include <citty/graphics/components/Vertex.hpp>

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

