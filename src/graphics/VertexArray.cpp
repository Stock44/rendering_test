//
// Created by hiram on 5/4/22.
//

#include "VertexArray.h"

#include <utility>

namespace graphics {

    void VertexArray::bind() {
        glBindVertexArray(ID);
    }

    VertexArray::VertexArray(GLuint ID, std::vector<std::shared_ptr<Buffer>>) : ID(ID), buffers(std::move(
            buffers)) {}
} // graphics