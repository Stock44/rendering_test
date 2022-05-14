//
// Created by hiram on 5/5/22.
//

#include "VertexArrayBuilder.h"

namespace graphics {
    VertexArrayBuilder::VertexArrayBuilder() = default;

    void VertexArrayBuilder::addBuffer(std::shared_ptr<Buffer> buffer) {
        buffers.insert(buffers.end(), buffer);

    }

    std::shared_ptr<VertexArray> VertexArrayBuilder::build() {
        GLuint ID = 0;
        glGenVertexArrays(1, &ID);

        glBindVertexArray(ID);
        for (auto &buffer: buffers) {
            buffer->enableAttribs();
        }
        glBindVertexArray(0);

        return std::make_shared<VertexArray>(ID, buffers);
    }
} // graphics