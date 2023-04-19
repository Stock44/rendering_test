//
// Created by hiram on 4/5/23.
//

#pragma once

#include <citty/graphics/OpenGlError.hpp>
#include <citty/graphics/Buffer.hpp>
#include <unordered_map>
#include <memory>
#include <any>

namespace citty::graphics {
    enum class AttributeType {
        FLOAT = GL_FLOAT,
    };

    enum class DrawMode {
        TRIANGLES = GL_TRIANGLES,
    };

    class VertexArray {
    public:
        VertexArray();

        VertexArray(VertexArray const &other) = delete;

        VertexArray &operator=(VertexArray const &other) = delete;

        VertexArray(VertexArray &&other) noexcept;

        VertexArray &operator=(VertexArray &&other) noexcept;

        void setVertexIndicesBuffer(std::shared_ptr<Buffer<unsigned int>> const &buffer);

        template<typename T>
        void bindBuffer(std::shared_ptr<Buffer<T>> const &buffer, std::size_t offset = 0) {
            if (offset != 0 && offset >= buffer->getSize()) {
                throw std::runtime_error("buffer offset is bigger than the buffer");
            }

            auto [bindingIt, inserted] = bufferBindings.try_emplace(buffer->getBufferName(), nextBufferBinding);
            auto [bufferName, binding] = *bindingIt;
            boundBuffers.try_emplace(binding, std::shared_ptr(buffer));

            nextBufferBinding++;
            glVertexArrayVertexBuffer(vertexArrayName, binding, buffer->getBufferName(), offset * sizeof(T), sizeof(T));
            checkOpenGlErrors();
        }

        template<typename T>
        void unbindBuffer(std::shared_ptr<Buffer<T>> const &buffer) {
            auto binding = bufferBindings.at(buffer->getBufferName());
            glVertexArrayVertexBuffer(vertexArrayName, binding, 0, 0, sizeof(T));
            checkOpenGlErrors();
            bufferBindings.erase(buffer->getBufferName());
            boundBuffers.erase(binding);
        }

        template<typename T>
        void configureAttrib(unsigned int attributeIndex, std::shared_ptr<Buffer<T>> const &buffer, int size,
                             AttributeType type, bool normalize, std::size_t offset) {
            if (sizeof(T) <= offset) {
                throw std::runtime_error("offset can't be bigger than the the size of the buffer's data type");
            }

            glEnableVertexArrayAttrib(vertexArrayName, attributeIndex);
            glVertexArrayAttribFormat(vertexArrayName, attributeIndex, size, asGlEnum(type), normalize, offset);
            glVertexArrayAttribBinding(vertexArrayName, attributeIndex, bufferBindings.at(buffer->getBufferName()));
            checkOpenGlErrors();
        }

        template<typename T>
        void setBufferDivisor(std::shared_ptr<Buffer<T>> const &buffer, int divisor) {
            glVertexArrayBindingDivisor(vertexArrayName, bufferBindings.at(buffer->getBufferName()), divisor);
            checkOpenGlErrors();
        }

        void enableAttrib(unsigned int attributeIndex);

        void disableAttrib(unsigned int attributeIndex);

        void drawElementsInstanced(DrawMode mode, int elementCount, int instanceCount,
                                   unsigned int indicesOffset = 0,
                                   int baseVertex = 0, int baseInstance = 0);

    private:
        unsigned int vertexArrayName = 0;

        void bind();

        // maps binding to buffers type-erased shared_ptrs to concrete Buffer<>
        // serves to ensure that buffers are not deleted while bound to any vertex array
        std::unordered_map<unsigned int, std::any> boundBuffers;

        // maps buffers to bindings
        std::unordered_map<unsigned int, unsigned int> bufferBindings;
        unsigned int nextBufferBinding = 0;
    };

} // graphics