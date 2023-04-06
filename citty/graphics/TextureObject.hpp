//
// Created by hiram on 4/5/23.
//

#pragma once

#include <epoxy/gl.h>
#include <type_traits>
#include "Image.hpp"

namespace citty::graphics {
    template <typename T>
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

    class TextureObject {
    public:
        explicit TextureObject(Image const &textureImage);

        TextureObject(TextureObject const &other) = delete;

        TextureObject &operator=(TextureObject const &other) = delete;

        TextureObject(TextureObject &&other) noexcept;

        TextureObject &operator=(TextureObject &&other) noexcept;

        ~TextureObject();

        void setTextureSWrapMode(WrapMode mode);

        void setTextureTWrapMode(WrapMode mode);

        void setTextureMinFilter(MinFilter filter);

        void setTextureMagFilter(MagFilter filter);

        void generateMipmaps();

        void bindToTextureUnit(unsigned int unit);

    protected:
        [[nodiscard]] unsigned int getTextureObjectName() const;

    private:
        unsigned int textureObjectName = 0;

    };

} // graphics