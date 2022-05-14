//
// Created by hiram on 4/24/22.
//

#ifndef CITYY_VERTEX_H
#define CITYY_VERTEX_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

namespace graphics {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;

        Vertex(float x, float y, float z) : position(x, y, z), normal(0.0f, 1.0f, 0.0f), texCoords(0.0f, 0.0f) {}
    };

    struct ColorVertex {
        glm::vec3 color;
    };

}
#endif //CITYY_VERTEX_H
