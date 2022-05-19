//
// Created by hiram on 5/4/22.
//

#include "VertexBuffer.h"

void graphics::VertexBuffer::setVertices(std::vector<Vertex> newVertices) {
    vertices = newVertices;
    dirty = true;
}

int graphics::VertexBuffer::getSize() {
    return vertices.size();
}

void graphics::VertexBuffer::modifyVertices(std::vector<Vertex> vertices, uint position) {
    // TODO implement pls
}

void graphics::VertexBuffer::addVertices(std::vector<Vertex> newVertices) {
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
    dirty = true;
}


void graphics::VertexBuffer::upload() {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    dirty = false;
}

void graphics::VertexBuffer::enableAttribs() {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(3);

}

void graphics::VertexBuffer::deleteVertex(int index) {
    vertices.erase(vertices.begin() + index);
    dirty = true;
};
