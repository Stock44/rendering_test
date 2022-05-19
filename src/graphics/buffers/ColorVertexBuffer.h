//
// Created by hiram on 5/18/22.
//

#ifndef CITYY_COLORVERTEXBUFFER_H
#define CITYY_COLORVERTEXBUFFER_H

#include <vector>
#include "Buffer.h"
#include "../types.h"

namespace graphics {

    class ColorVertexBuffer : public Buffer{
    public:
        void enableAttribs() override;

        int getSize() override;

        void addVertices(std::vector<Color> newVertices);

        void addVertex(Color newVertex);

        void setVertices(std::vector<Color> newVertices);

        void modifyVertices(std::vector<Color> modifiedVertices, uint position);

        void deleteVertex(int index);

        void upload() override;
    private:
        std::vector<Color> vertices;
    };

} // graphics

#endif //CITYY_COLORVERTEXBUFFER_H
