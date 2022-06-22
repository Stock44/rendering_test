//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_VERTEXARRAY_H
#define CITYY_VERTEXARRAY_H

#include <memory>
#include <vector>
#include <span>
#include <map>
#include "glad/glad.h"
#include "buffers/Buffer.h"

namespace graphics {

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();
        void bind() const;
        void registerBuffer(Buffer *buffer);
        void registerBuffer(Buffer &buffer);
    private:
        GLuint ID;
    };

} // graphics

#endif //CITYY_VERTEXARRAY_H
