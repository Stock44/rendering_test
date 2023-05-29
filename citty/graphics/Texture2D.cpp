//
// Created by hiram on 4/23/23.
//

#include <citty/graphics/Texture2D.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include <citty/graphics/AsGlEnum.hpp>
#include "SizedImageFormat.hpp"

namespace citty::graphics {
    Texture2D::Texture2D(SizedImageFormat format, int width, int height) {
        if (width < 1 || height < 1) {
            throw std::runtime_error("2d texture width and height cannot be less than 1");
        }

        glTextureStorage2D(getName(), 1, asGlEnum(format), width, height);
        checkOpenGlErrors();
    }

    Texture2D::Texture2D(Image const &image) {
        SizedImageFormat internalFormat;
        TextureFormat imageFormat;

        switch (image.getChannels()) {
            case ColorChannels::G:
                internalFormat = SizedImageFormat::R8;
                imageFormat = TextureFormat::R;
                break;
            case ColorChannels::GA:
                internalFormat = SizedImageFormat::RG8;
                imageFormat = TextureFormat::RG;
                break;
            case ColorChannels::RGB:
                internalFormat = SizedImageFormat::RGB8;
                imageFormat = TextureFormat::RGB;
                break;
            case ColorChannels::RGBA:
                internalFormat = SizedImageFormat::RGBA8;
                imageFormat = TextureFormat::RGBA;
                break;
        }

        glTextureStorage2D(getName(), 1, asGlEnum(internalFormat), image.getWidth(), image.getHeight());
        checkOpenGlErrors();

        glTextureSubImage2D(getName(), 0, 0, 0, image.getWidth(), image.getHeight(), asGlEnum(imageFormat),
                            GL_UNSIGNED_BYTE,
                            image.getData());
        checkOpenGlErrors();
    }
} // graphics