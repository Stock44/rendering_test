//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_NODE_H
#define CITYY_NODE_H

#include <utility>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace map {
    class Road;

    class Node {
    public:
        Node(int id, glm::vec3 position);

        [[nodiscard]] const glm::vec3 &getPosition() const;

        void setPosition(glm::vec3 newPosition);

        [[nodiscard]] const std::vector<std::weak_ptr<Road>> & getParentRoads() const;

        void addParentRoad(std::weak_ptr<Road> parentRoad);

        int getId() const;

    private:
        glm::vec3 position;
        int id;
        std::vector<std::weak_ptr<Road>> parentRoads;

    };

    using NodePtr = std::shared_ptr<Node>;

} // map

#endif //CITYY_NODE_H
