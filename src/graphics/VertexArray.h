//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_VERTEXARRAY_H
#define CITYY_VERTEXARRAY_H

#include <memory>
#include <vector>
#include "glad/glad.h"
#include "buffers/Buffer.h"

namespace graphics {

    class VertexArray {
    public:
        VertexArray(GLuint ID, std::vector<std::shared_ptr<Buffer>>);
        void bind();
    private:
        GLuint ID = 0;
        std::vector<std::shared_ptr<Buffer>> buffers;
    };

} // graphics

#endif //CITYY_VERTEXARRAY_H
