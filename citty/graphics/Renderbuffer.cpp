//
// Created by hiram on 4/24/23.
//

#include "AsGlEnum.hpp"
#include <citty/graphics/OpenGlError.hpp>
#include <citty/graphics/Renderbuffer.hpp>

namespace citty::graphics {
Renderbuffer::Renderbuffer(SizedImageFormat format, int width, int height) {
  glCreateRenderbuffers(1, &name);
  glNamedRenderbufferStorage(name, asGlEnum(format), width, height);
  checkOpenGlErrors();
}

Renderbuffer::Renderbuffer(Renderbuffer &&other) noexcept : name(other.name) {
  other.name = 0;
}

Renderbuffer &Renderbuffer::operator=(Renderbuffer &&other) noexcept {
  if (this == &other)
    return *this;

  glDeleteRenderbuffers(1, &name);
  name = other.name;
  other.name = 0;

  return *this;
}

Renderbuffer::~Renderbuffer() { glDeleteRenderbuffers(1, &name); }

unsigned int Renderbuffer::getName() const { return name; }
} // namespace citty::graphics
