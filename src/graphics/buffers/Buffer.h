//
// Created by hiram on 5/5/22.
//

#ifndef CITYY_BUFFER_H
#define CITYY_BUFFER_H

#include <cstddef>
#include "glad/glad.h"

namespace graphics {

    class Buffer {
    public:
        Buffer();

        virtual ~Buffer() = default;

        [[nodiscard]] virtual long getSize() const = 0;

        virtual void enableAttribs() = 0;

        virtual void upload() = 0;

    protected:
        [[nodiscard]] GLuint getID() const;

    private:
        GLuint ID = 0;
    };

} // graphics

#endif //CITYY_BUFFER_H
