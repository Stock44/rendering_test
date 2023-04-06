//
// Created by hiram on 4/5/23.
//

#include <citty/graphics/VertexArray.hpp>
#include "OpenGlError.hpp"

namespace citty::graphics {
    VertexArray::VertexArray() {
        glCreateVertexArrays(1, &vertexArrayName);
    }

    VertexArray::VertexArray(VertexArray &&other) noexcept {
        vertexArrayName = other.vertexArrayName;
        other.vertexArrayName = 0;
    }

    VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
        if (vertexArrayName == other.vertexArrayName) {
            return *this;
        }

        vertexArrayName = other.vertexArrayName;
        other.vertexArrayName = 0;

        return *this;
    }

    void VertexArray::setVertexIndicesBuffer(Buffer<unsigned int> &buffer) {
        glVertexArrayElementBuffer(vertexArrayName, buffer.getBufferName());
    }

    void VertexArray::enableAttrib(unsigned int attributeIndex) {
        glEnableVertexArrayAttrib(vertexArrayName, attributeIndex);
    }

    void VertexArray::disableAttrib(unsigned int attributeIndex) {
        glDisableVertexArrayAttrib(vertexArrayName, attributeIndex);
    }

} // graphics