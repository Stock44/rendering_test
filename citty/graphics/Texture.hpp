//
// Created by hiram on 4/5/23.
//

#pragma once

#include <epoxy/gl.h>
#include <type_traits>
#include <citty/graphics/Image.hpp>
#include <Eigen/Dense>

namespace citty::graphics {
    template<typename T>
    int asParam(T value) {
        return static_cast<std::underlying_type_t<T>>(value);
    }

    enum class WrapMode {
        REPEAT = GL_REPEAT,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
    };

    enum class MagFilter {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
    };

    enum class MinFilter {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
    };

    enum class TextureFormat {
        R = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
    };

    enum class PixelType {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    };

    class Texture {
    public:
        Texture();

        Texture(Texture &&other) noexcept;

        Texture &operator=(Texture &&other) noexcept;

        ~Texture();

        void setSWrapMode(WrapMode mode);

        void setTWrapMode(WrapMode mode);

        void setMinFilter(MinFilter filter);

        void setMagFilter(MagFilter filter) const;

        void setBorderColor(Eigen::Vector4f color);

        void generateMipmaps();

        void bindToTextureUnit(unsigned int unit) const;

        [[nodiscard]] unsigned int getName() const;

        static void unbindTextureUnit(int unit);

    private:
        unsigned int name = 0;

    };

} // graphics