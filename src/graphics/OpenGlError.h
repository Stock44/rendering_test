//
// Created by hiram on 6/22/22.
//

#ifndef CITYY_OPENGLERROR_H
#define CITYY_OPENGLERROR_H

#include <stdexcept>

namespace graphics {

    class OpenGLError : public std::runtime_error {
    public:
        explicit OpenGLError(unsigned int errorCode);
        [[nodiscard]] unsigned int getErrorCode() const;

    private:
        unsigned int errorCode;
    };

} // graphics

#endif //CITYY_OPENGLERROR_H
