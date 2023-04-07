//
// Created by hiram on 3/30/23.
//

#pragma once

#include<string>
#include <citty/graphics/TextureObject.hpp>

namespace citty::graphics {


    struct Texture {
        std::string texturePath;
        WrapMode sWrappingMode = WrapMode::REPEAT;
        WrapMode tWrappingMode = WrapMode::REPEAT;
        MinFilter minFilter = MinFilter::NEAREST;
        MagFilter magFilter = MagFilter::NEAREST;
    };
}