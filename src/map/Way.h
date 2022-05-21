//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_WAY_H
#define CITYY_WAY_H

#include <memory>
#include <vector>

namespace map {

    class Node;
    class Way {
    public:

        void addNode(std::shared_ptr<Node> node);

        [[nodiscard]] const std::vector<std::shared_ptr<Node>> &getNodes() const;

        void setNodes(const std::vector<std::shared_ptr<Node>> &nodes);

    private:
        std::vector<std::shared_ptr<Node>> nodes;
    };

} // map

#endif //CITYY_WAY_H
