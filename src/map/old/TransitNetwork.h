//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_TRANSITNETWORK_H
#define CITYY_TRANSITNETWORK_H

#include <unordered_map>
#include <map>
#include "Road.h"
#include "types.h"

namespace map {

    class TransitNetwork {
    public:
        TransitNetwork(NodeMap nodes, HighwayMap highways);
        void addRoad(RoadPtr road);
        void addNode();
        const NodeMap & getNodes();
        const HighwayMap &getHighways();
        int getNodeCount();
        int getHighwayCount();
        // O(n) complexity to the number of nodes
        NodePtr getClosestNodeToCoord(glm::vec3 coord);
    private:
        HighwayMap highways;
        NodeMap nodes;
    };

} // map

#endif //CITYY_TRANSITNETWORK_H