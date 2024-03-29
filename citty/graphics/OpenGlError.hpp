//
// Created by hiram on 6/22/22.
//

#pragma once

#include <stdexcept>

namespace citty::graphics {

    class OpenGLError : public std::runtime_error {
    public:
        explicit OpenGLError(unsigned int errorCode);
        [[nodiscard]] unsigned int getErrorCode() const;

    private:
        unsigned int errorCode;
    };

    void checkOpenGlErrors();

} // graphics
