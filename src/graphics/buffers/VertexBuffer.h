//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_VERTEXBUFFER_H
#define CITYY_VERTEXBUFFER_H


#include <vector>
#include "Buffer.h"
#include "../components/Vertex.h"

namespace graphics {
    class VertexBuffer : public Buffer{
    public:
        [[nodiscard]] long getSize() const override;

        void enableAttribs() override;

        void addVertices(std::vector<Vertex> const & newVertices);

        void setVertices(std::vector<Vertex> &&newVertices);

        void setVertices(std::vector<Vertex> const &newVertices);

        void modifyVertices(std::vector<Vertex> modVertices, long modPosition);

        void deleteVertex(long position);

        void upload() override;
    private:
        std::vector<Vertex> vertices;
    };
}


#endif //CITYY_VERTEXBUFFER_H
