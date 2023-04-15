//
// Created by hiram on 4/5/23.
//

#include <citty/graphics/Texture.hpp>
#include <citty/graphics/OpenGlError.hpp>

namespace citty::graphics {
    Texture::Texture(Image const &image) {
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

    Texture::Texture(Texture &&other) noexcept {
        textureObjectName = other.textureObjectName;
        other.textureObjectName = 0;
    }

    Texture &Texture::operator=(Texture &&other) noexcept {
        if (textureObjectName == other.textureObjectName) {
            return *this;
        }

        textureObjectName = other.textureObjectName;
        other.textureObjectName = 0;
        return *this;
    }

    Texture::~Texture() {
        glDeleteTextures(1, &textureObjectName);
    }

    unsigned int graphics::Texture::getTextureName() const {
        return textureObjectName;
    }

    void Texture::setTextureSWrapMode(WrapMode mode) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_WRAP_S, asParam(mode));
        checkOpenGlErrors();
    }

    void Texture::setTextureTWrapMode(WrapMode mode) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_WRAP_T, asParam(mode));
        checkOpenGlErrors();
    }

    void Texture::setTextureMinFilter(MinFilter filter) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_MIN_FILTER, asParam(filter));
        checkOpenGlErrors();
    }

    void Texture::setTextureMagFilter(MagFilter filter) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_MAG_FILTER, asParam(filter));
        checkOpenGlErrors();
    }

    void Texture::generateMipmaps() {
        glGenerateTextureMipmap(textureObjectName);
        checkOpenGlErrors();
    }

    void Texture::bindToTextureUnit(unsigned int unit) {
        glBindTextureUnit(unit, textureObjectName);
        checkOpenGlErrors();
    }

} // graphics