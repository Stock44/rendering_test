//
// Created by hiram on 4/4/23.
//

#pragma once

#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/BufferUsage.hpp>

namespace citty::graphics {

    enum class AttributeType {
        FLOAT = GL_FLOAT,
    };

    auto asGlEnum(AttributeType arrayType) {
        return static_cast<std::underlying_type_t<AttributeType>>(arrayType);
    }

    class VertexAttributesBuffer : Buffer {
        void configureAttribute(unsigned int attributeIndex, int size, AttributeType type, bool normalize,
                                int stride, std::size_t offset);

        void enableAttribute(unsigned int attributeIndex);

        void setAttributeDivisor(unsigned int attributeIndex, int divisor);

        template<std::ranges::contiguous_range R>
        requires std::is_standard_layout_v<std::ranges::range_value_t<R>>
        void setData(R data, BufferUsage usage) {
            glBindBuffer(GL_ARRAY_BUFFER, bufferName);

            glBufferData(GL_ARRAY_BUFFER, std::ranges::size(data) * sizeof(std::ranges::range_value_t<R>),
                         std::ranges::data(data), asGlEnum(usage));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

    };

} // graphics