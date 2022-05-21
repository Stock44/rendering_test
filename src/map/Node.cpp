//
// Created by hiram on 5/17/22.
//

#include "Node.h"

namespace map {
    const glm::vec3 &Node::getCoords() const {
        return coords;
    }

    Node::Node(int id, const glm::vec3 coords) : id(id), coords(coords){

    }

    void Node::setCoords(glm::vec3 &coords) {
        Node::coords = coords;
    }

    const std::vector<std::weak_ptr<Way>> &Node::getParentWays() const {
        return parentWays;
    }

    void Node::addParentWay(std::weak_ptr<Way> parentWay) {
        parentWays.push_back(parentWay);
    }

    int Node::getId() const {
        return id;
    }

} // map