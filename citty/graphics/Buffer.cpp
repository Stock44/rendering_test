//
// Created by hiram on 5/5/22.
//

#include<citty/graphics/Buffer.hpp>
#include <epoxy/gl.h>

namespace citty::graphics {
    Buffer::Buffer(Buffer &&other) noexcept {
        bufferName = other.bufferName;
        other.bufferName = 0;
    }

    Buffer &Buffer::operator=(Buffer &&other) noexcept {
        if (bufferName == other.bufferName) {
            return *this;
        }

        bufferName = other.bufferName;
        other.bufferName = 0;

        return *this;
    }

    Buffer::~Buffer() {
        glDeleteBuffers(1, &bufferName);
    }



} // graphics