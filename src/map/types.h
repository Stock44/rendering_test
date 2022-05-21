//
// Created by hiram on 5/18/22.
//

#ifndef CITYY_MAP_TYPES_H
#define CITYY_MAP_TYPES_H

#include <memory>
#include <unordered_map>
#include "Node.h"
#include "Highway.h"

namespace map {
    // Basic pointer types
    typedef std::shared_ptr<Node> NodePtr;
    typedef std::shared_ptr<Highway> HighwayPtr;

    // ID to element mapping storage
    typedef std::unordered_map<int, NodePtr> NodeMap;
    typedef std::unordered_map<int, HighwayPtr> HighwayMap;

    // Lane definition for a highway, first is lanes in the way's direction, second is in the reverse direection
}

#endif //CITYY_MAP_TYPES_H
