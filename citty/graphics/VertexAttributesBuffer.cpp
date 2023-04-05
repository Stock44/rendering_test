//
// Created by hiram on 4/4/23.
//

#include <citty/graphics/VertexAttributesBuffer.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include <epoxy/gl.h>

namespace citty::graphics {
    void VertexAttributesBuffer::configureAttribute(unsigned int attributeIndex, int size, AttributeType type,
                                                    bool normalize,
                                                    GLsizei stride, std::size_t offset) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferName);
        glVertexAttribPointer(attributeIndex, size, static_cast<std::underlying_type_t<AttributeType>>(type),
                              normalize,
                              stride, (const void *) offset);
        auto error = glGetError();
        if (error) throw OpenGLError(error);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexAttributesBuffer::enableAttribute(unsigned int attributeIndex) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferName);

        glEnableVertexAttribArray(attributeIndex);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexAttributesBuffer::setAttributeDivisor(unsigned int attributeIndex, int divisor) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferName);

        glVertexAttribDivisor(attributeIndex, divisor);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
} // graphics