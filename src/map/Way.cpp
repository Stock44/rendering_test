//
// Created by hiram on 5/17/22.
//

#include "Way.h"

namespace map {
    const std::vector<std::shared_ptr<Node>> &Way::getNodes() const {
        return nodes;
    }

    void Way::setNodes(const std::vector<std::shared_ptr<Node>> &nodes) {
        Way::nodes = nodes;
    }

    void Way::addNode(std::shared_ptr<Node> node) {
        nodes.push_back(node);
    }
} // map