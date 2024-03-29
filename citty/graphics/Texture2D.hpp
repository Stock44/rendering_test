//
// Created by hiram on 4/23/23.
//

#pragma once

#include <citty/graphics/Texture.hpp>
#include <citty/graphics/AsGlEnum.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include <citty/graphics/SizedImageFormat.hpp>


namespace citty::graphics {
    class Texture2D : public Texture {
    public:
        Texture2D(SizedImageFormat format, int width, int height);

        explicit Texture2D(Image const &image);
    };

} // graphics