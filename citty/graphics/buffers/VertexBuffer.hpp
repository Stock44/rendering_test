//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_VERTEXBUFFER_HPP
#define CITYY_VERTEXBUFFER_HPP


#include <vector>
#include <citty/graphics/buffers/Buffer.hpp>
#include <citty/graphics/components/Vertex.hpp>

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


#endif //CITYY_VERTEXBUFFER_HPP
