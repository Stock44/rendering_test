//
// Created by hiram on 5/5/22.
//

#pragma once

#include <cstddef>

namespace citty::graphics {
    class Buffer {
    public:
        Buffer(Buffer const &other) = delete;

        Buffer &operator=(Buffer const &other) = delete;

        Buffer(Buffer &&other) noexcept;

        Buffer &operator=(Buffer &&other) noexcept;

        ~Buffer();

    private:
        unsigned int bufferName = 0;
    };

} // graphics
