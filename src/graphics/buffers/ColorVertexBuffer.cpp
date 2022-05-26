//
// Created by hiram on 5/18/22.
//

#include "ColorVertexBuffer.h"

#include <utility>

namespace graphics {
    void ColorVertexBuffer::enableAttribs() {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Color), nullptr);
        glEnableVertexAttribArray(7);
        glVertexAttribDivisor(7, 1);
    }

    int ColorVertexBuffer::getSize() {
        return vertices.size();
    }

    void ColorVertexBuffer::addVertices(std::vector<Color> newVertices) {
        vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
        dirty = true;
    }

    void ColorVertexBuffer::setVertices(std::vector<Color> newVertices) {
        vertices = std::move(newVertices);
        dirty = true;
    }

    void ColorVertexBuffer::modifyVertices(std::vector<Color> modifiedVertices, uint position) {
        // TODO implement pls
    }

    void ColorVertexBuffer::upload() {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Color), &vertices[0], GL_STATIC_DRAW);
        dirty = false;
    }

    void ColorVertexBuffer::addVertex(Color newVertex) {
        vertices.insert(vertices.end(), newVertex);
        dirty = true;
    }

    void ColorVertexBuffer::deleteVertex(int index) {
        vertices.erase(vertices.begin() + index);
        dirty = true;
    }
} // graphics