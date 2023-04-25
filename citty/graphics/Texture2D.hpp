//
// Created by hiram on 4/23/23.
//

#pragma once

#include <citty/graphics/Texture.hpp>
#include <citty/graphics/AsGlEnum.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include "SizedImageFormat.hpp"


namespace citty::graphics {
    class Texture2D : public Texture {
    public:
        void reallocate(SizedImageFormat format, int width, int height);


        void setImage(Image const &image);

    };

} // graphics