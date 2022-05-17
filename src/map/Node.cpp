//
// Created by hiram on 5/17/22.
//

#include "Node.h"

namespace map {
    const glm::vec3 &Node::getCoords() const {
        return coords;
    }

    Node::Node(const glm::vec3 coords) : coords(coords){

    }

    void Node::setCoords(glm::vec3 &coords) {
        Node::coords = coords;
    }
} // map