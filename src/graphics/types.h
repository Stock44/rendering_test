//
// Created by hiram on 4/24/22.
//

#ifndef CITYY_GRAPHICS_TYPES_H
#define CITYY_GRAPHICS_TYPES_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <memory>

namespace graphics {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;

         Vertex(float x, float y, float z) noexcept : position(x, y, z), normal(0.0f, 1.0f, 0.0f), texCoords(0.0f, 0.0f) {}

    };

    typedef std::pair<std::string, int> ObjectID;
    typedef std::pair<uint, uint> ModelLoc;

    typedef glm::vec4 Color;
}
#endif //CITYY_GRAPHICS_TYPES_H
