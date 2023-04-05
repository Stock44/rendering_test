//
// Created by hiram on 6/22/22.
//

#include <epoxy/gl.h>
#include <citty/graphics/OpenGlError.hpp>

namespace citty::graphics {
    OpenGLError::OpenGLError(unsigned int errorCode) : std::runtime_error("OpenGL error " + std::to_string(errorCode)),
                                              errorCode(errorCode) {}

    unsigned int OpenGLError::getErrorCode() const {
        return errorCode;
    }

    void checkOpenGlErrors() {
        auto error = glGetError();
        if (error) throw OpenGLError(error);
    }
} // graphics