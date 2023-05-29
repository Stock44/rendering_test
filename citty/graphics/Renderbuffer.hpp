//
// Created by hiram on 4/24/23.
//

#pragma once

#include <epoxy/gl.h>
#include "SizedImageFormat.hpp"

namespace citty::graphics {
    class Renderbuffer {
    public:
        Renderbuffer(SizedImageFormat format, int width, int height);

        Renderbuffer(Renderbuffer &&other) noexcept;

        Renderbuffer &operator=(Renderbuffer &&other) noexcept;

        ~Renderbuffer();

        [[nodiscard]] unsigned int getName() const;

    private:
        unsigned int name = 0;

    };
} // graphics