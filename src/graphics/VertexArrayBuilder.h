//
// Created by hiram on 5/5/22.
//

#ifndef CITYY_VERTEXARRAYBUILDER_H
#define CITYY_VERTEXARRAYBUILDER_H

#include "VertexArray.h"

namespace graphics {

    class VertexArrayBuilder {
    public:
        VertexArrayBuilder();

        void addBuffer(std::shared_ptr<Buffer> buffer);

        std::shared_ptr<VertexArray> build();
    private:
        std::vector<std::shared_ptr<Buffer>> buffers;

    };

} // graphics

#endif //CITYY_VERTEXARRAYBUILDER_H
