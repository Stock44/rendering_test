//
// Created by hiram on 5/4/22.
//

#include "VertexArray.h"

namespace graphics {

    void VertexArray::bind() const {
        glBindVertexArray(ID);
    }

    VertexArray::VertexArray() { // NOLINT(cppcoreguidelines-pro-type-member-init)
        glGenVertexArrays(1, &ID);
    }

    void VertexArray::registerBuffer(Buffer *buffer) {
        glBindVertexArray(ID);

        buffer->enableAttribs();

        glBindVertexArray(0);
    }

    void VertexArray::registerBuffer(Buffer &buffer) {
        glBindVertexArray(ID);

        buffer.enableAttribs();

        glBindVertexArray(0);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &ID);
    }
} // graphics