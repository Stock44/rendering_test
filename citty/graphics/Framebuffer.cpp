//
// Created by hiram on 4/22/23.
//


#include <epoxy/gl.h>
#include <citty/graphics/Framebuffer.hpp>

namespace citty::graphics {
    Framebuffer::Framebuffer() {
        glCreateFramebuffers(1, &name);
    }

    Framebuffer::Framebuffer(Framebuffer &&other) noexcept: name(other.name) {
        other.name = 0;
    }

    Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
        name = other.name;
        other.name = 0;
        return *this;
    }

    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &name);
    }

    unsigned int Framebuffer::getName() const {
        return name;
    }

    void Framebuffer::addTexture(Texture2D const &texture, AttachmentType type, int level) {
        glNamedFramebufferTexture(name, asGlEnum(type), texture.getName(), level);
        checkOpenGlErrors();
    }

    void Framebuffer::setNoDrawBuffer() {
        glNamedFramebufferDrawBuffer(name, GL_NONE);
        checkOpenGlErrors();
    }

    void Framebuffer::setDrawBuffer(int attachment) {
        glNamedFramebufferDrawBuffer(name, GL_COLOR_ATTACHMENT0 + attachment);
        checkOpenGlErrors();
    }

    void Framebuffer::setNoReadBuffer() {
        glNamedFramebufferReadBuffer(name, GL_NONE);
        checkOpenGlErrors();
    }

    void Framebuffer::setReadBuffer(int attachment) {
        glNamedFramebufferReadBuffer(name, GL_COLOR_ATTACHMENT0 + attachment);
        checkOpenGlErrors();
    }

    void Framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, name);
        checkOpenGlErrors();
    }

    void Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkOpenGlErrors();
    }

    void Framebuffer::addRenderbuffer(Renderbuffer const &renderbuffer, AttachmentType type) {
        glNamedFramebufferRenderbuffer(name, asGlEnum(type), GL_RENDERBUFFER, renderbuffer.getName());
        checkOpenGlErrors();
    }
} // graphics