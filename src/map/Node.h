//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_NODE_H
#define CITYY_NODE_H

#include <utility>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>
#include "Way.h"


namespace map {

    class Node {
    public:
        Node(int id, glm::vec3 coords);

        [[nodiscard]] const glm::vec3 &getCoords() const;

        void setCoords(glm::vec3 &coords);

        [[nodiscard]] const std::vector<std::weak_ptr<Way>> &getParentWays() const;

        void addParentWay(std::weak_ptr<Way> parent);

        int getId() const;

    private:
        glm::vec3 coords;
        int id;
        std::vector<std::weak_ptr<Way>> parentWays;

    };

} // map

#endif //CITYY_NODE_H
