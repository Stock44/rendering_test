//
// Created by hiram on 4/5/23.
//

#include <citty/graphics/TextureObject.hpp>
#include "OpenGlError.hpp"

namespace citty::graphics {
    TextureObject::TextureObject(Image const &image) {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureObjectName);
        checkOpenGlErrors();

        GLenum internalFormat;
        GLenum imageFormat;
        switch (image.getChannels()) {
            case ColorChannels::G:
                internalFormat = GL_R8;
                imageFormat = GL_RED;
                break;
            case ColorChannels::GA:
                internalFormat = GL_RG8;
                imageFormat = GL_RG;
                break;
            case ColorChannels::RGB:
                internalFormat = GL_RGB8;
                imageFormat = GL_RGB;
                break;
            case ColorChannels::RGBA:
                internalFormat = GL_RGBA8;
                imageFormat = GL_RGBA;
                break;
        }

        glTextureStorage2D(textureObjectName, 1, internalFormat, image.getWidth(), image.getHeight());
        checkOpenGlErrors();
        glTextureSubImage2D(textureObjectName, 0, 0, 0, image.getWidth(), image.getHeight(), imageFormat,
                            GL_UNSIGNED_BYTE,
                            image.getData());
        checkOpenGlErrors();
    }

    TextureObject::TextureObject(TextureObject &&other) noexcept {
        textureObjectName = other.textureObjectName;
        other.textureObjectName = 0;
    }

    TextureObject &TextureObject::operator=(TextureObject &&other) noexcept {
        if (textureObjectName == other.textureObjectName) {
            return *this;
        }

        textureObjectName = other.textureObjectName;
        other.textureObjectName = 0;
        return *this;
    }

    TextureObject::~TextureObject() {
        glDeleteTextures(1, &textureObjectName);
    }

    unsigned int graphics::TextureObject::getTextureObjectName() const {
        return textureObjectName;
    }

    void TextureObject::setTextureSWrapMode(WrapMode mode) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_WRAP_S, asParam(mode));
        checkOpenGlErrors();
    }

    void TextureObject::setTextureTWrapMode(WrapMode mode) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_WRAP_T, asParam(mode));
        checkOpenGlErrors();
    }

    void TextureObject::setTextureMinFilter(MinFilter filter) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_MIN_FILTER, asParam(filter));
        checkOpenGlErrors();
    }

    void TextureObject::setTextureMagFilter(MagFilter filter) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_MAG_FILTER, asParam(filter));
        checkOpenGlErrors();
    }

    void TextureObject::generateMipmaps() {
        glGenerateTextureMipmap(textureObjectName);
        checkOpenGlErrors();
    }

    void TextureObject::bindToTextureUnit(unsigned int unit) {
        glBindTextureUnit(unit, textureObjectName);
        checkOpenGlErrors();
    }

} // graphics