//
// Created by hiram on 5/6/22.
//

#include "IndexBuffer.h"

namespace graphics {
    int IndexBuffer::getSize() {
        return indices.size();
    }

    void IndexBuffer::enableAttribs() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    }

    void IndexBuffer::addIndices(std::vector<uint> newIndices) {
        indices.insert(indices.end(), newIndices.begin(), newIndices.end());
        dirty = true;
    }

    void IndexBuffer::setIndices(std::vector<uint> newIndices) {
        indices = newIndices;
        dirty = true;
    }

    void IndexBuffer::modifyIndices(std::vector<uint> modifiedIndices, uint position) {
        // TODO implement this
    }

    void IndexBuffer::upload() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
        dirty = false;
    }

    void IndexBuffer::addIndex(uint index) {
        indices.push_back(index);
        dirty = true;
    }
} // graphics