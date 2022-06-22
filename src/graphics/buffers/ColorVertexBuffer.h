//
// Created by hiram on 5/18/22.
//

#ifndef CITYY_COLORVERTEXBUFFER_H
#define CITYY_COLORVERTEXBUFFER_H

#include <vector>
#include <glm/detail/type_vec3.hpp>
#include "Buffer.h"
#include "../components/Color.h"

namespace graphics {

    class ColorVertexBuffer : public Buffer{
    public:
        [[nodiscard]] long getSize() const override;

        void enableAttribs() override;

        void addVertex(Color newVertex);

        void addVertices(std::vector<Color> const &newVertices);

        void setVertices(std::vector<Color> &&newVertices);

        void setVertices(std::vector<Color> const &newVertices);

        void modifyVertices(std::vector<Color> modVertices, long modPosition);

        void deleteVertex(long position);

        void upload() override;

        void setVertex(Color newColor, long modPosition);

    private:
        std::vector<Color> vertices;
    };

} // graphics

#endif //CITYY_COLORVERTEXBUFFER_H
