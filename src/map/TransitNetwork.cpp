//
// Created by hiram on 5/17/22.
//

#include "TransitNetwork.h"

#include <utility>

namespace map {
    void TransitNetwork::addHighway(HighwayPtr highway) {
    }

    TransitNetwork::TransitNetwork(NodeMap nodes, HighwayMap highways) : nodes(std::move(nodes)),
                                                                         highways(std::move(highways)) {}

    void TransitNetwork::addNode() {

    }

    int TransitNetwork::getNodeCount() {
        return nodes.size();
    }

    int TransitNetwork::getHighwayCount() {
        return highways.size();
    }

    const NodeMap & TransitNetwork::getNodes() {
        return nodes;
    }

    const HighwayMap &TransitNetwork::getHighways() {
        return highways;
    }
} // map