//
// Created by hiram on 4/5/23.
//

#pragma once

#include <citty/graphics/OpenGlError.hpp>
#include <citty/graphics/Buffer.hpp>
#include <unordered_map>

namespace citty::graphics {
    enum class AttributeType {
        FLOAT = GL_FLOAT,
    };

    auto asGlEnum(AttributeType arrayType) {
        return static_cast<std::underlying_type_t<AttributeType>>(arrayType);
    }

    class VertexArray {
    public:
        VertexArray();

        VertexArray(VertexArray const &other) = delete;

        VertexArray &operator=(VertexArray const &other) = delete;

        VertexArray(VertexArray &&other) noexcept;

        VertexArray &operator=(VertexArray &&other) noexcept;

        void setVertexIndicesBuffer(Buffer<unsigned int> &buffer);

        template<typename T>
        void bindBuffer(Buffer<T> &buffer) {
            unsigned int binding = bufferBindings.try_emplace(buffer.getBufferName(), nextBufferBinding);
            nextBufferBinding++;

            glVertexArrayVertexBuffer(vertexArrayName, binding, buffer.getBufferName(), 0, sizeof(T));
            checkOpenGlErrors();
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

        std::unordered_map<unsigned int, unsigned int> bufferBindings;
        unsigned int nextBufferBinding = 0;
    };

} // graphics