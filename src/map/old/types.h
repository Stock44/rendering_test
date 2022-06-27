//
// Created by hiram on 5/18/22.
//

#ifndef CITYY_MAP_TYPES_H
#define CITYY_MAP_TYPES_H

#include <memory>
#include <unordered_map>
#include "Node.h"
#include "Road.h"

namespace map {
    // Basic pointer types
    typedef std::shared_ptr<Node> NodePtr;
    typedef std::shared_ptr<Road> HighwayPtr;

    // ID to element mapping storage
    typedef std::unordered_map<long, NodePtr> NodeMap;
    typedef std::unordered_map<long, HighwayPtr> HighwayMap;

    // Lane definition for a highway, first is lanes in the way's direction, second is in the reverse direection
}

#endif //CITYY_MAP_TYPES_H