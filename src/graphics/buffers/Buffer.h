//
// Created by hiram on 5/5/22.
//

#ifndef CITYY_BUFFER_H
#define CITYY_BUFFER_H

#include "glad/glad.h"

namespace graphics {

    class Buffer {
    public:
        Buffer();

        virtual int getSize() = 0;

        virtual void enableAttribs() = 0;

        virtual void upload() = 0;

        bool isDirty();

    protected:
        bool dirty = false;
        GLuint ID = 0;
    };

} // graphics

#endif //CITYY_BUFFER_H
