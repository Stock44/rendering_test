//
// Created by hiram on 6/27/22.
//

#pragma once

#include <vector>
#include <citty/engine/Entity.hpp>

namespace map {

    struct Node {
        std::vector<engine::Entity> parentRoads;
    };

} // map
