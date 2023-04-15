//
// Created by hiram on 4/12/23.
//

#pragma once

#include <citty/graphics/Texture.hpp>

namespace citty::graphics {
    struct TextureSettings {
        WrapMode sWrapMode = WrapMode::REPEAT;
        WrapMode tWrapMode = WrapMode::REPEAT;
        MinFilter minFilter = MinFilter::NEAREST;
        MagFilter magFilter = MagFilter::NEAREST;
    };
}
