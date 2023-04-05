//
// Created by hiram on 4/4/23.
//

#pragma once

#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/BufferUsage.hpp>

namespace citty::graphics {
    class VertexIndicesBuffer : Buffer {
        template<std::ranges::contiguous_range R>
        requires std::is_same_v<std::ranges::range_value_t<R>, unsigned int>
        void setData(R data, BufferUsage usage) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferName);

            glBufferData(GL_ARRAY_BUFFER, std::ranges::size(data) * sizeof(unsigned int),
                         std::ranges::data(data), asGlEnum(usage));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    };
}


