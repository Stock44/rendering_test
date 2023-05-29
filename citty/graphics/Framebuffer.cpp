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

    void Framebuffer::setColorAttachment(std::shared_ptr<Texture2D> texture2D, unsigned int position, int level) {
        glNamedFramebufferTexture(name, GL_COLOR_ATTACHMENT0 + position, texture2D->getName(), level);
        checkOpenGlErrors();
        if (colorAttachments.contains(position)) {
            colorAttachments.at(position) = std::move(texture2D);
        } else {
            colorAttachments.try_emplace(position, std::move(texture2D));
        }
    }

    void Framebuffer::setColorAttachment(std::shared_ptr<Renderbuffer> renderbuffer, unsigned int position) {
        glNamedFramebufferRenderbuffer(name, GL_COLOR_ATTACHMENT0 + position, GL_RENDERBUFFER, renderbuffer->getName());
        checkOpenGlErrors();
        if (colorAttachments.contains(position)) {
            colorAttachments.at(position) = std::move(renderbuffer);
        } else {
            colorAttachments.try_emplace(position, std::move(renderbuffer));
        }
    }

    void Framebuffer::setDepthAttachment(std::shared_ptr<Texture2D> texture2D, int level) {
        glNamedFramebufferTexture(name, GL_DEPTH_ATTACHMENT, texture2D->getName(), level);
        checkOpenGlErrors();
        depthAttachment = std::move(texture2D);
    }

    void Framebuffer::setDepthAttachment(std::shared_ptr<Renderbuffer> renderbuffer) {
        glNamedFramebufferRenderbuffer(name, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer->getName());
        checkOpenGlErrors();
        depthAttachment = std::move(renderbuffer);
    }
} // graphics