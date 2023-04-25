//
// Created by hiram on 4/22/23.
//

#pragma once

#include <citty/graphics/Texture2D.hpp>
#include <citty/graphics/Renderbuffer.hpp>

namespace citty::graphics {

    enum class AttachmentType {
        DEPTH = GL_DEPTH_ATTACHMENT,
        STENCIL = GL_STENCIL_ATTACHMENT,
        DEPTH_STENCIL = GL_DEPTH_STENCIL,
    };

    class Framebuffer {
    public:
        Framebuffer();

        Framebuffer(Framebuffer const &other) = delete;

        Framebuffer &operator=(Framebuffer const &other) = delete;

        Framebuffer(Framebuffer &&other) noexcept;

        Framebuffer &operator=(Framebuffer &&other) noexcept;

        void bind();

        void addTexture(Texture2D const &texture, AttachmentType type, int level = 0);

        void addRenderbuffer(Renderbuffer const &renderbuffer, AttachmentType type);

        void setNoDrawBuffer();

        void setDrawBuffer(int attachment);

        void setNoReadBuffer();

        void setReadBuffer(int attachment);

        [[nodiscard]] unsigned int getName() const;

        ~Framebuffer();

        static void unbind();

    private:
        unsigned int name = 0;
    };

} // graphics