//
// Created by hiram on 4/24/23.
//

#include <citty/graphics/Renderbuffer.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include "AsGlEnum.hpp"

namespace citty::graphics {
    Renderbuffer::Renderbuffer(SizedImageFormat format, int width, int height) {
        glCreateRenderbuffers(1, &name);
        glNamedRenderbufferStorage(name, asGlEnum(format), width, height);
        checkOpenGlErrors();
    }

    Renderbuffer::Renderbuffer(Renderbuffer &&other) noexcept: name(other.name) {
        other.name = 0;
    }

    Renderbuffer &Renderbuffer::operator=(Renderbuffer &&other) noexcept {
        if (other.name == this->name) return *this;

        name = other.name;
        other.name = 0;

        return *this;
    }

    Renderbuffer::~Renderbuffer() {
        glDeleteRenderbuffers(1, &name);
    }

    unsigned int Renderbuffer::getName() const {
        return name;
    }
} // graphics