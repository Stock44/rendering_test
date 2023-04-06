//
// Created by hiram on 4/5/23.
//

#pragma once

#include <epoxy/gl.h>
#include <type_traits>

namespace citty::graphics {
    template<typename T>
    GLenum asGlEnum(T type) {
        return static_cast<std::underlying_type_t<T>>(type);
    }
}

