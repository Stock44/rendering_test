//
// Created by hiram on 4/5/23.
//

#include <citty/graphics/TextureObject.hpp>

namespace citty::graphics {
    TextureObject::TextureObject(Image const &image) {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureObjectName);

        GLenum color;
        switch (image.getChannels()) {
            case ColorChannels::G:
                color = GL_R8;
                break;
            case ColorChannels::GA:
                color = GL_RG8;
                break;
            case ColorChannels::RGB:
                color = GL_RGB8;
                break;
            case ColorChannels::RGBA:
                color = GL_RGBA8;
                break;
        }

        glTextureStorage2D(textureObjectName, 1, color, image.getWidth(), image.getHeight());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.getWidth(), image.getHeight(), color, GL_UNSIGNED_BYTE,
                        image.getData());
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
    }

    void TextureObject::setTextureTWrapMode(WrapMode mode) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_WRAP_T, asParam(mode));
    }

    void TextureObject::setTextureMinFilter(MinFilter filter) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_MIN_FILTER, asParam(filter));
    }

    void TextureObject::setTextureMagFilter(MagFilter filter) {
        glTextureParameteri(textureObjectName, GL_TEXTURE_MAG_FILTER, asParam(filter));
    }

    void TextureObject::generateMipmaps() {
        glGenerateTextureMipmap(textureObjectName);
    }

    void TextureObject::bindToTextureUnit(unsigned int unit) {
        glBindTextureUnit(unit, textureObjectName);
    }

} // graphics