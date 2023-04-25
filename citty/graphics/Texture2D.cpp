//
// Created by hiram on 4/23/23.
//

#include <citty/graphics/Texture2D.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include <citty/graphics/AsGlEnum.hpp>
#include "SizedImageFormat.hpp"

namespace citty::graphics {
    void Texture2D::reallocate(SizedImageFormat format, int width, int height) {
        glTextureStorage2D(getName(), 1, asGlEnum(format), width, height);
        checkOpenGlErrors();
    }

    void Texture2D::setImage(Image const &image) {
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

        reallocate(internalFormat, image.getWidth(), image.getHeight());

        glTextureSubImage2D(getName(), 0, 0, 0, image.getWidth(), image.getHeight(), asGlEnum(imageFormat),
                            GL_UNSIGNED_BYTE,
                            image.getData());
        checkOpenGlErrors();
    }

} // graphics