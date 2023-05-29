//
// Created by hiram on 4/5/23.
//

#include <citty/graphics/Texture.hpp>
#include <citty/graphics/OpenGlError.hpp>

namespace citty::graphics {
    Texture::Texture() {
        glCreateTextures(GL_TEXTURE_2D, 1, &name);
    }

    Texture::Texture(Texture &&other) noexcept {
        name = other.name;
        other.name = 0;
    }

    Texture &Texture::operator=(Texture &&other) noexcept {
        if (name == other.name) {
            return *this;
        }

        name = other.name;
        other.name = 0;
        return *this;
    }

    Texture::~Texture() {
        glDeleteTextures(1, &name);
    }

    unsigned int graphics::Texture::getName() const {
        return name;
    }

    void Texture::setSWrapMode(WrapMode mode) {
        glTextureParameteri(name, GL_TEXTURE_WRAP_S, asParam(mode));
        checkOpenGlErrors();
    }

    void Texture::setTWrapMode(WrapMode mode) {
        glTextureParameteri(name, GL_TEXTURE_WRAP_T, asParam(mode));
        checkOpenGlErrors();
    }

    void Texture::setMinFilter(MinFilter filter) {
        glTextureParameteri(name, GL_TEXTURE_MIN_FILTER, asParam(filter));
        checkOpenGlErrors();
    }

    void Texture::setMagFilter(MagFilter filter) const {
        glTextureParameteri(name, GL_TEXTURE_MAG_FILTER, asParam(filter));
        checkOpenGlErrors();
    }

    void Texture::generateMipmaps() {
        glGenerateTextureMipmap(name);
        checkOpenGlErrors();
    }

    void Texture::bindToTextureUnit(unsigned int unit) const {
        glBindTextureUnit(unit, name);
        checkOpenGlErrors();
    }

    void Texture::setBorderColor(Eigen::Vector4f color) {
        glTextureParameterfv(name, GL_TEXTURE_BORDER_COLOR, color.data());
        checkOpenGlErrors();
    }

    void Texture::unbindTextureUnit(int unit) {
        glBindTextureUnit(unit, 0);
        checkOpenGlErrors();
    }

} // graphics