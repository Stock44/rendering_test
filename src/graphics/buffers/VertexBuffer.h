//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_VERTEXBUFFER_H
#define CITYY_VERTEXBUFFER_H


#include <vector>
#include "../types.h"
#include "Buffer.h"

namespace graphics {
    class VertexBuffer : public Buffer{
    public:
        void enableAttribs() override;

        int getSize() override;

        void addVertices(std::vector<Vertex> newVertices);

        void setVertices(std::vector<Vertex> newVertices);

        void modifyVertices(std::vector<Vertex> modifiedVertices, uint position);

        void deleteVertex(int index);

        void upload() override;
    private:
        std::vector<Vertex> vertices;
    };
}


#endif //CITYY_VERTEXBUFFER_H
