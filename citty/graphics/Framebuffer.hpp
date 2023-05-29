//
// Created by hiram on 4/22/23.
//

#pragma once

#include <citty/graphics/Texture2D.hpp>
#include <citty/graphics/Renderbuffer.hpp>
#include <vector>
#include <variant>
#include <optional>

namespace citty::graphics {

    class Framebuffer {
    public:
        Framebuffer();

        Framebuffer(Framebuffer const &other) = delete;

        Framebuffer &operator=(Framebuffer const &other) = delete;

        Framebuffer(Framebuffer &&other) noexcept;

        Framebuffer &operator=(Framebuffer &&other) noexcept;

        void bind();

        void setColorAttachment(std::shared_ptr<Texture2D> texture2D, unsigned int position, int level = 0);

        void setColorAttachment(std::shared_ptr<Renderbuffer> renderbuffer, unsigned int position);

        void setDepthAttachment(std::shared_ptr<Texture2D> texture2D, int level = 0);

        void setDepthAttachment(std::shared_ptr<Renderbuffer> renderbuffer);

        void setNoDrawBuffer();

        void setDrawBuffer(int attachment);

        void setNoReadBuffer();

        void setReadBuffer(int attachment);

        [[nodiscard]] unsigned int getName() const;

        ~Framebuffer();

        static void unbind();

    private:
        unsigned int name = 0;
        using Attachment = std::variant<std::shared_ptr<Texture2D>, std::shared_ptr<Renderbuffer>>;
        std::optional<Attachment> depthAttachment;
        std::unordered_map<unsigned int, std::optional<Attachment>> colorAttachments;
    };

} // graphics