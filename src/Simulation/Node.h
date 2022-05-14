//
// Created by hiram on 4/26/22.
//

#ifndef CITYY_NODE_H
#define CITYY_NODE_H

#include <vector>

namespace sim {
    struct Node {
        Node(float x, float y, float z);

        float x;
        float y;
        float z;

        std::vector<unsigned int> incoming;
        std::vector<unsigned int> outgoing;

        void addOutgoingRoad(unsigned int destinationNode);

        void removeOutgoingRoad(unsigned int destinationNode);

        void addIncomingRoad(unsigned int sourceNode);

        void removeIncomingRoad(unsigned int sourceNode);
    };
}


#endif //CITYY_NODE_H
