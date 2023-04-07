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

    class VertexArray {
    public:
        VertexArray();

        VertexArray(VertexArray const &other) = delete;

        VertexArray &operator=(VertexArray const &other) = delete;

        VertexArray(VertexArray &&other) noexcept;

        VertexArray &operator=(VertexArray &&other) noexcept;

        void setVertexIndicesBuffer(Buffer<unsigned int> &buffer);

        template<typename T>
        void bindBuffer(std::shared_ptr<Buffer<T>> const &buffer, std::size_t offset = 0) {
            unsigned int binding = bufferBindings.try_emplace(buffer->getBufferName(), nextBufferBinding);
            boundBuffers.try_emplace(nextBufferBinding, std::shared_ptr(buffer));

            nextBufferBinding++;
            glVertexArrayVertexBuffer(vertexArrayName, binding, buffer->getBufferName(), offset, sizeof(T));
            checkOpenGlErrors();
        }

        template<typename T>
        void unbindBuffer(std::shared_ptr<Buffer<T>> const &buffer) {
            auto binding = bufferBindings.at(buffer->getBufferName());
            glVertexArrayVertexBuffer(vertexArrayName, binding, 0, 0, sizeof(T));
            bufferBindings.erase(buffer->getBufferName());
            boundBuffers.erase(binding);
        }

        template<typename T>
        void configureAttrib(unsigned int attributeIndex, Buffer<T> const &buffer, int size,
                             AttributeType type, bool normalize, std::size_t offset) {
            static_assert(sizeof(T) > offset, "offset can not be bigger than the buffer's data type");

            glEnableVertexArrayAttrib(vertexArrayName, attributeIndex);
            glVertexArrayAttribFormat(vertexArrayName, attributeIndex, size, asGlEnum(type), normalize, offset);
            glVertexArrayAttribBinding(vertexArrayName, attributeIndex, bufferBindings.at(buffer.getBufferName()));
            checkOpenGlErrors();
        }

        template<typename T>
        void setBufferDivisor(Buffer<T> const &buffer, int divisor) {
            glVertexArrayBindingDivisor(vertexArrayName, bufferBindings.at(buffer.getBufferName()), divisor);
        }

        void enableAttrib(unsigned int attributeIndex);

        void disableAttrib(unsigned int attributeIndex);

    private:
        unsigned int vertexArrayName = 0;

        // maps binding to buffers type-erased shared_ptrs to concrete Buffer<>
        // serves to ensure that buffers are not deleted while bound to any vertex array
        std::unordered_map<unsigned int, std::any> boundBuffers;

        // maps buffers to bindings
        std::unordered_map<unsigned int, unsigned int> bufferBindings;
        unsigned int nextBufferBinding = 0;
    };

} // graphics