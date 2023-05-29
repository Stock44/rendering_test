//
// Created by hiram on 5/5/22.
//

#pragma once

#include <ranges>
#include <citty/graphics/AsGlEnum.hpp>
#include <epoxy/gl.h>
#include <citty/graphics/OpenGlError.hpp>

namespace citty::graphics {
    enum class BufferUsage {
        STATIC_DRAW = GL_STATIC_DRAW,
        STATIC_READ = GL_STATIC_READ,
        STATIC_COPY = GL_STATIC_COPY,
        STREAM_DRAW = GL_STREAM_DRAW,
        STREAM_READ = GL_STREAM_READ,
        STREAM_COPY = GL_STREAM_COPY,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        DYNAMIC_READ = GL_DYNAMIC_READ,
        DYNAMIC_COPY = GL_DYNAMIC_COPY,
    };

    enum class BufferTarget {
        SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
    };

    template<typename T> requires std::is_standard_layout_v<T>
    class Buffer {
    public:
        using DataType = T;

        explicit Buffer(BufferUsage usage, std::size_t initialSize = 0) : bufferUsage(usage) {
            glCreateBuffers(1, &name);
            checkOpenGlErrors();

            // First call to resize initializes the buffer
            reallocate(initialSize, usage);
        }

        Buffer(Buffer const &other) = delete;

        Buffer &operator=(Buffer const &other) = delete;

        Buffer(Buffer &&other) noexcept: name(other.name), size(other.size), bufferUsage(other.bufferUsage) {
            other.name = 0;
            other.size = 0;
        }

        Buffer &operator=(Buffer &&other) noexcept {
            if (name == other.name) {
                return *this;
            }

            name = other.name;
            bufferUsage = other.bufferUsage;
            size = other.size;
            other.name = 0;
            other.size = 0;

            return *this;
        }

        ~Buffer() {
            glDeleteBuffers(1, &name);
        }

        void reallocate(std::size_t elementCount, BufferUsage usage) {
            glNamedBufferData(name, elementCount * sizeof(T), nullptr, asGlEnum(usage));
            checkOpenGlErrors();
            size = elementCount;
            bufferUsage = usage;
        }

        void append(std::span<T const> data) {
            unsigned int tempBuffer;
            std::size_t newSize = size + data.size();
            glCreateBuffers(1, &tempBuffer);
            glNamedBufferData(tempBuffer, newSize * sizeof(T), nullptr,
                              asGlEnum(bufferUsage));
            glCopyNamedBufferSubData(name, tempBuffer, 0, 0, size * sizeof(T));
            glNamedBufferSubData(tempBuffer, size * sizeof(T), data.size() * sizeof(T),
                                 data.data());
            checkOpenGlErrors();

            reallocate(newSize, bufferUsage);
            glCopyNamedBufferSubData(tempBuffer, name, 0, 0, size * sizeof(T));

            glDeleteBuffers(1, &tempBuffer);
            checkOpenGlErrors();
        }

        void bindToTarget(unsigned int index, BufferTarget target) {
            glBindBufferBase(asGlEnum(target), index, name);
            checkOpenGlErrors();
        }


        /**
         * Reallocates the buffer to exactly fit the elements of the data range. The new allocation is performed
         * using the given buffer usage hint.
         * @tparam R
         * @param data
         * @param usage
         */
        void setData(std::span<T const> data, BufferUsage usage) {
            glNamedBufferData(name, data.size() * sizeof(T),
                              data.data(), asGlEnum(usage));
            checkOpenGlErrors();
            size = data.size();
            bufferUsage = usage;
        }

        void setSubData(std::span<T const> data, std::size_t offset = 0) {
            if (offset + data.size() > size) {
                throw std::runtime_error("data does not fit in buffer");
            }
            glNamedBufferSubData(name, offset, data.size() * sizeof(T), data.data());
            checkOpenGlErrors();
        }

        [[nodiscard]] unsigned int getBufferName() const {
            return name;
        }

        [[nodiscard]] std::size_t getSize() const {
            return size;
        }

    private:
        BufferUsage bufferUsage = BufferUsage::STATIC_DRAW;
        unsigned int name = 0;
        std::size_t size = 0;
    };

} // graphics
