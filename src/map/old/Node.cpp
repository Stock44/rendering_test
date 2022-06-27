//
// Created by hiram on 5/17/22.
//

#include "Node.h"

namespace map {
    const glm::vec3 &Node::getPosition() const {
        return position;
    }

    Node::Node(int id, const glm::vec3 position) : position(position), id(id){

    }

    void Node::setPosition(glm::vec3 newPosition) {
        Node::position = newPosition;
    }

    const std::vector<std::weak_ptr<Road>> & Node::getParentRoads() const {
        return parentRoads;
    }

    void Node::addParentRoad(std::weak_ptr<Road> parentRoad) {
        parentRoads.push_back(parentRoad);
    }

    int Node::getId() const {
        return id;
    }

} // map