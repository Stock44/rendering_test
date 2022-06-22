//
// Created by hiram on 6/6/22.
//

#ifndef CITYY_VERTEX_H
#define CITYY_VERTEX_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace graphics {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal = {0.0f, 1.0f, 0.0f};
        glm::vec2 texCoords = {0.0f, 0.0f};

        Vertex(float x, float y, float z) noexcept: position(x, y, z) {}

    };
}
#endif //CITYY_VERTEX_H
