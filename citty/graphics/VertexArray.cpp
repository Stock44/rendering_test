//
// Created by hiram on 4/5/23.
//

#include <citty/graphics/VertexArray.hpp>

namespace citty::graphics {
    VertexArray::VertexArray() {
        glCreateVertexArrays(1, &vertexArrayName);
    }

    VertexArray::VertexArray(VertexArray &&other) noexcept {
        vertexArrayName = other.vertexArrayName;
        boundBuffers = std::move(other.boundBuffers);
        bufferBindings = std::move(other.bufferBindings);
        nextBufferBinding = other.nextBufferBinding;
        other.nextBufferBinding = 0;
        other.vertexArrayName = 0;
    }

    VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
        if (vertexArrayName == other.vertexArrayName) {
            return *this;
        }

        vertexArrayName = other.vertexArrayName;
        boundBuffers = std::move(other.boundBuffers);
        bufferBindings = std::move(other.bufferBindings);
        nextBufferBinding = other.nextBufferBinding;
        other.nextBufferBinding = 0;
        other.vertexArrayName = 0;

        return *this;
    }

    void VertexArray::setVertexIndicesBuffer(std::shared_ptr<Buffer<unsigned int>> const &buffer) {
        glVertexArrayElementBuffer(vertexArrayName, buffer->getBufferName());
        checkOpenGlErrors();
    }

    void VertexArray::enableAttrib(unsigned int attributeIndex) {
        glEnableVertexArrayAttrib(vertexArrayName, attributeIndex);
        checkOpenGlErrors();
    }

    void VertexArray::disableAttrib(unsigned int attributeIndex) {
        glDisableVertexArrayAttrib(vertexArrayName, attributeIndex);
        checkOpenGlErrors();
    }

    void VertexArray::drawElementsInstanced(DrawMode mode, int elementCount, int instanceCount,
                                            unsigned int indicesOffset, int baseVertex, int baseInstance) {
        bind();
        glDrawElementsInstancedBaseVertexBaseInstance(asGlEnum(mode), elementCount, GL_UNSIGNED_INT,
                                                      (void const *) (indicesOffset * sizeof(unsigned int)),
                                                      instanceCount,
                                                      baseVertex, baseInstance);
        checkOpenGlErrors();
    }

    void VertexArray::bind() {
        glBindVertexArray(vertexArrayName);
        checkOpenGlErrors();
    }

} // graphics