//
// Created by hiram on 4/24/23.
//

#pragma once

#include <epoxy/gl.h>
#include "SizedImageFormat.hpp"

namespace citty::graphics {
    class Renderbuffer {
    public:
        Renderbuffer();

        Renderbuffer(Renderbuffer &&other) noexcept;

        Renderbuffer &operator=(Renderbuffer &&other) noexcept;

        ~Renderbuffer();

        void reallocate(SizedImageFormat format, int width, int height);

        [[nodiscard]] unsigned int getName() const;

    private:
        unsigned int name = 0;

    };
} // graphics