//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_VERTEXARRAY_HPP
#define CITYY_VERTEXARRAY_HPP

#include <memory>
#include <vector>
#include <span>
#include <map>
#include <optional>
#include <citty/glad/glad.h>
#include <citty/graphics/buffers/Buffer.hpp>

namespace graphics {

    class VertexArray {
    public:
        VertexArray();

        ~VertexArray();

        VertexArray(VertexArray const &other) = delete;

        VertexArray(VertexArray &&other) noexcept;

        VertexArray &operator=(VertexArray &&other) noexcept;

        VertexArray &operator=(VertexArray const &other) = delete;

        void bind() const;

    private:
        std::optional<GLuint> ID;
    };

} // graphics

#endif //CITYY_VERTEXARRAY_HPP
