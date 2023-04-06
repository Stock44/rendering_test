//
// Created by hiram on 5/5/22.
//

#pragma once

#include <cstddef>
#include <ranges>
#include <citty/graphics/BufferUsage.hpp>
#include <citty/graphics/AsGlEnum.hpp>
#include <epoxy/gl.h>

namespace citty::graphics {
    template<typename T> requires std::is_standard_layout_v<T>
    class Buffer {
    public:
        using DataType = T;

        Buffer() {
            glCreateBuffers(1, &bufferName);
        }

        Buffer(Buffer const &other) = delete;

        Buffer &operator=(Buffer const &other) = delete;

        Buffer(Buffer &&other) noexcept {
            bufferName = other.bufferName;
            other.bufferName = 0;
        }

        Buffer &operator=(Buffer &&other) noexcept {
            if (bufferName == other.bufferName) {
                return *this;
            }

            bufferName = other.bufferName;
            other.bufferName = 0;

            return *this;
        }

        ~Buffer() {
            glDeleteBuffers(1, &bufferName);
        }

        template<std::ranges::contiguous_range R>
        requires std::is_same_v<std::ranges::range_value_t<R>, T>
        void setData(R data, BufferUsage usage) {
            glNamedBufferData(bufferName, std::ranges::size(data) * sizeof(std::ranges::range_value_t<R>),
                              std::ranges::data(data), asGlEnum(usage));
        }

        [[nodiscard]] unsigned int getBufferName() const {
            return bufferName;
        }

    private:
        unsigned int bufferName = 0;
    };

} // graphics
