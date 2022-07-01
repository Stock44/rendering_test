//
// Created by hiram on 6/27/22.
//

#ifndef CITYY_NODE_H
#define CITYY_NODE_H

#include <vector>
#include "../../engine/Entity.h"

namespace map {

    struct Node {
        std::vector<engine::Entity> parentRoads;
    };

} // map

#endif //CITYY_NODE_H
