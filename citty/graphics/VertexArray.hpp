//
// Created by hiram on 5/4/22.
//

#pragma once

#include <memory>
#include <vector>
#include <span>
#include <map>
#include <optional>
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
        std::optional<unsigned int> ID;
    };

} // graphics
