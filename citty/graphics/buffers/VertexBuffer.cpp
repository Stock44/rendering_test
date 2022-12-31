//
// Created by hiram on 5/4/22.
//

#include <citty/graphics/buffers/VertexBuffer.hpp>
#include <citty/graphics/OpenGlError.hpp>
#include <stdexcept>
#include <algorithm>
#include <epoxy/gl.h>

long graphics::VertexBuffer::getSize() const {
    return std::ssize(vertices);
}

void graphics::VertexBuffer::enableAttribs() {
    glBindBuffer(GL_ARRAY_BUFFER, getID().value());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(3);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::VertexBuffer::addVertices(std::vector<Vertex> const &newVertices) {
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());

}


void graphics::VertexBuffer::setVertices(std::vector<Vertex> &&newVertices) {
    vertices = std::move(newVertices);
}

void graphics::VertexBuffer::setVertices(std::vector<Vertex> const &newVertices) {
    vertices = newVertices;
}


void graphics::VertexBuffer::modifyVertices(std::vector<Vertex> modVertices, long modPosition) {
    if (modVertices.empty()) return;
    if (modPosition > std::ssize(vertices)) throw std::invalid_argument("Position is out of bounds");
    if (std::ssize(modVertices) > std::ssize(vertices) - modPosition) throw std::invalid_argument("Too many vertices to modify");
    // Pointer to the vertex before the first element to modify
    auto swapStart = std::next(std::begin(vertices), modPosition);

    std::ranges::swap_ranges(swapStart, std::next(swapStart, std::ssize(modVertices)), std::begin(modVertices),
                             std::end(modVertices));
}

void graphics::VertexBuffer::upload() {
    glBindBuffer(GL_ARRAY_BUFFER, getID().value());
    glBufferData(GL_ARRAY_BUFFER, std::ssize(vertices) * static_cast<long>(sizeof(Vertex)), &vertices[0],
                 GL_STATIC_DRAW);
    auto error = glGetError();
    if (error) throw OpenGLError(error);
}

void graphics::VertexBuffer::deleteVertex(long index) {
    vertices.erase(std::next(std::begin(vertices), index));
}
