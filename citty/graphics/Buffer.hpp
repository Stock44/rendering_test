//
// Created by hiram on 5/5/22.
//

#pragma once

#include <ranges>
#include <citty/graphics/AsGlEnum.hpp>
#include <epoxy/gl.h>
#include "OpenGlError.hpp"

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

    template<typename R, typename T>
    concept BufferMappableRange = requires {
        requires std::ranges::sized_range<R>;
        requires std::ranges::sized_range<R>;
        requires std::is_same_v<std::ranges::range_value_t<R>, T>;
    };

    template<typename T> requires std::is_standard_layout_v<T>
    class Buffer {
    public:
        using DataType = T;

        explicit Buffer(BufferUsage usage, std::size_t initialSize = 0) : bufferUsage(usage) {
            glCreateBuffers(1, &bufferName);
            checkOpenGlErrors();

            // First call to resize initializes the buffer
            reallocate(initialSize, usage);
        }

        Buffer(Buffer const &other) = delete;

        Buffer &operator=(Buffer const &other) = delete;

        Buffer(Buffer &&other) noexcept {
            bufferName = other.bufferName;
            bufferUsage = other.bufferUsage;
            other.bufferName = 0;
        }

        Buffer &operator=(Buffer &&other) noexcept {
            if (bufferName == other.bufferName) {
                return *this;
            }

            bufferName = other.bufferName;
            bufferUsage = other.bufferUsage;
            other.bufferName = 0;

            return *this;
        }

        ~Buffer() {
            glDeleteBuffers(1, &bufferName);
        }

        void reallocate(std::size_t elementCount, BufferUsage usage) {
            glNamedBufferData(bufferName, elementCount * sizeof(T), nullptr, asGlEnum(usage));
            checkOpenGlErrors();
            size = elementCount;
            bufferUsage = usage;
        }

        template<typename R>
        requires BufferMappableRange<R, T>
        void append(R &data) {
            unsigned int newBufferName;
            glCreateBuffers(1, &newBufferName);
            glNamedBufferData(newBufferName, (size + std::ranges::size(data)) * sizeof(T), nullptr,
                              asGlEnum(bufferUsage));
            glCopyNamedBufferSubData(bufferName, newBufferName, 0, 0, size * sizeof(T));

            checkOpenGlErrors();

            glDeleteBuffers(1, &bufferName);
            checkOpenGlErrors();
            bufferName = newBufferName;
            std::size_t oldSize = size;
            size = size + std::ranges::size(data);

            setSubData(data, oldSize);
        }

        /**
         * Reallocates the buffer to exactly fit the elements of the data range. The new allocation is performed
         * using the given buffer usage hint.
         * @tparam R
         * @param data
         * @param usage
         */
        template<typename R>
        requires BufferMappableRange<R, T>
        void setData(R &data, BufferUsage usage) {
            glNamedBufferData(bufferName, std::ranges::size(data) * sizeof(T),
                              std::ranges::data(data), asGlEnum(usage));
            checkOpenGlErrors();
            size = std::ranges::size(data);
            bufferUsage = usage;
        }

        template<typename R>
        requires BufferMappableRange<R, T>
        void setSubData(R &data, std::size_t offset = 0) {
            if (offset + std::ranges::size(data) > size) {
                throw std::runtime_error("data does not fit in buffer");
            }
            glNamedBufferSubData(bufferName, offset, std::ranges::size(data) * sizeof(T), std::ranges::data(data));
            checkOpenGlErrors();
        }

        [[nodiscard]] unsigned int getBufferName() const {
            return bufferName;
        }

        [[nodiscard]] std::size_t getSize() const {
            return size;
        }

    private:
        BufferUsage bufferUsage = BufferUsage::STATIC_DRAW;
        unsigned int bufferName = 0;
        std::size_t size = 0;
    };

} // graphics
