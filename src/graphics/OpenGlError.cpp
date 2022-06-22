//
// Created by hiram on 6/22/22.
//

#include "OpenGlError.h"

namespace graphics {
    OpenGLError::OpenGLError(unsigned int errorCode) : std::runtime_error("OpenGL error " + std::to_string(errorCode)),
                                              errorCode(errorCode) {}

    unsigned int OpenGLError::getErrorCode() const {
        return errorCode;
    }
} // graphics