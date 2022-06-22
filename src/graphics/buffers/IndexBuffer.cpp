//
// Created by hiram on 5/6/22.
//

#include <algorithm>
#include <stdexcept>
#include "IndexBuffer.h"

namespace graphics {
    long IndexBuffer::getSize() const {
        return std::ssize(indices);
    }

    void IndexBuffer::enableAttribs() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getID());
    }

    void IndexBuffer::addIndex(uint index) {
        indices.emplace_back(index);
    }

    void IndexBuffer::addIndices(std::vector<uint> const &newIndices) {
        indices.insert(indices.end(), newIndices.begin(), newIndices.end());
    }

    void IndexBuffer::setIndices(std::vector<uint> const &newIndices) {
        indices = newIndices;
    }

    void IndexBuffer::setIndices(std::vector<uint> &&newIndices) {
        indices = std::move(newIndices);
    }

    void IndexBuffer::modifyIndices(std::vector<uint> modIndices, long modStart) {
        if (modStart > std::ssize(indices)) throw std::invalid_argument("Position is out of bounds");
        if (std::ssize(modIndices) > std::ssize(indices) - modStart)
            throw std::invalid_argument("Too many indices to modify");
        if (modIndices.empty()) return;

        auto swapStart = std::next(std::begin(indices), modStart);

        std::ranges::swap_ranges(swapStart, std::next(swapStart, std::ssize(modIndices)), std::begin(modIndices),
                                 std::end(modIndices));
    }

    void IndexBuffer::upload() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getID());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::ssize(indices) * static_cast<long>(sizeof(uint)), &indices[0],
                     GL_STATIC_DRAW);
    }
} // graphics