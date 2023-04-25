//
// Created by hiram on 4/24/23.
//

#pragma once

#include <epoxy/gl.h>
#include <type_traits>
#include <citty/graphics/Image.hpp>

enum class SizedImageFormat {
    R8 = GL_R8,
    RG8 = GL_RG8,
    RGB8 = GL_RGB8,
    RGBA8 = GL_RGBA8,
    DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
    DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,
    DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32,
};