//
// Created by hiram on 5/18/22.
//

#include "ColorVertexBuffer.h"

#include <utility>
#include <algorithm>
#include <stdexcept>

namespace graphics {
    long ColorVertexBuffer::getSize() const {
        return std::ssize(vertices);
    }

    void ColorVertexBuffer::enableAttribs() {
        glBindBuffer(GL_ARRAY_BUFFER, getID());
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Color), nullptr);
        glEnableVertexAttribArray(7);
        glVertexAttribDivisor(7, 1);
    }

    void ColorVertexBuffer::addVertices(std::vector<Color> const &newVertices) {
        vertices.insert(std::end(vertices), std::begin(newVertices), std::end(newVertices));
    }

    void ColorVertexBuffer::setVertices(std::vector<Color> const &newVertices) {
        vertices = newVertices;
    }

    void ColorVertexBuffer::setVertices(std::vector<Color> &&newVertices) {
        vertices = std::move(newVertices);
    }

    void ColorVertexBuffer::modifyVertices(std::vector<Color> modVertices, long modPosition) {
        if (modVertices.empty()) return;
        if (modPosition > std::ssize(vertices)) throw std::invalid_argument("Position is out of bounds!");
        if (std::ssize(modVertices) > std::ssize(vertices) - modPosition)
            throw std::invalid_argument("Too many vertices to modify");

        auto swapStart = std::next(std::begin(vertices), modPosition);

        std::ranges::swap_ranges(swapStart, std::next(swapStart, std::ssize(modVertices)), std::begin(modVertices),
                                 std::end(modVertices));

    }

    void ColorVertexBuffer::upload() {
        glBindBuffer(GL_ARRAY_BUFFER, getID());
        glBufferData(GL_ARRAY_BUFFER, std::ssize(vertices) * static_cast<long>(sizeof(Color)), &vertices[0],
                     GL_STATIC_DRAW);
    }

    void ColorVertexBuffer::addVertex(Color newVertex) {
        vertices.insert(std::begin(vertices), newVertex);
    }

    void ColorVertexBuffer::deleteVertex(long index) {
        vertices.erase(std::begin(vertices) + index);
    }

    void ColorVertexBuffer::setVertex(Color newColor, long modPosition) {
        vertices.at(modPosition) = newColor;
    }
} // graphics