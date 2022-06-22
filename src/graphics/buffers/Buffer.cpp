//
// Created by hiram on 5/5/22.
//

#include "Buffer.h"

namespace graphics {
    Buffer::Buffer() {
        glGenBuffers(1, &ID);
    }

    GLuint Buffer::getID() const {
        return ID;
    }
} // graphics