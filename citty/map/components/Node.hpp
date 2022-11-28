//
// Created by hiram on 6/27/22.
//

#ifndef CITYY_NODE_HPP
#define CITYY_NODE_HPP

#include <vector>
#include <citty/engine/Entity.hpp>

namespace map {

    struct Node {
        std::vector<engine::Entity> parentRoads;
    };

} // map

#endif //CITYY_NODE_HPP
