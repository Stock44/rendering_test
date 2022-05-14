//
// Created by hiram on 5/6/22.
//

#ifndef CITYY_INDEXBUFFER_H
#define CITYY_INDEXBUFFER_H

#include <vector>
#include <cstdlib>
#include "Buffer.h"

namespace graphics {

    class IndexBuffer : public Buffer {
    public:
        int getSize() override;

        void enableAttribs() override;

        void addIndices(std::vector<uint> newIndices);

        void addIndex(uint index);

        void setIndices(std::vector<uint> newIndices);

        void modifyIndices(std::vector<uint> modifiedIndices, uint position);

        void upload() override;

    private:
        std::vector<uint> indices;
    };

} // graphics

#endif //CITYY_INDEXBUFFER_H
