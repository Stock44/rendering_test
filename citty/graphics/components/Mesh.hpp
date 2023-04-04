//
// Created by hiram on 6/6/22.
//
#pragma once

#include <citty/graphics/components/Vertex.hpp>
#include <vector>

namespace citty::graphics {
    struct Mesh {
        std::vector<Vertex> const vertices;
        // unsigned int instead of std::size_t, as OpenGL uses this type for mesh indices
        std::vector<unsigned int> const indices;
        engine::Entity material;
    };
}

