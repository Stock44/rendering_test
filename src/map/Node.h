//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_NODE_H
#define CITYY_NODE_H

#include <utility>
#include <glm/vec3.hpp>

namespace map {

    class Node {
    public:
        Node(glm::vec3 coords);

        [[nodiscard]] const glm::vec3 &getCoords() const;

        void setCoords(glm::vec3 &coords);

    private:
        glm::vec3 coords;
    };

} // map

#endif //CITYY_NODE_H
