//
// Created by hiram on 4/26/22.
//

#include "Node.h"

sim::Node::Node(float x, float y, float z) : x(x), y(y), z(z) {}

void sim::Node::addOutgoingRoad(unsigned int destinationNode) {
    outgoing.insert(outgoing.end(), destinationNode);
}

void sim::Node::removeOutgoingRoad(unsigned int destinationNode)  {
    for (auto it = outgoing.begin(); it != outgoing.end(); it++) {
        if (*it == destinationNode) {
            outgoing.erase(it);
            return;
        }
    }
}

void sim::Node::addIncomingRoad(unsigned int sourceNode)  {
    incoming.insert(incoming.end(), sourceNode);
}

void sim::Node::removeIncomingRoad(unsigned int sourceNode)  {
    for (auto it = incoming.begin(); it != incoming.end(); it++) {
        if (*it == sourceNode) {
            incoming.erase(it);
            return;
        }
    }
}