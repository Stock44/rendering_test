//
// Created by hiram on 5/17/22.
//

#ifndef CITYY_ROAD_H
#define CITYY_ROAD_H

#include <optional>
#include "RoadType.h"
#include "Node.h"

namespace map {
    using Lanes = std::pair<int, int>;

    class Road {
    public:
        Road(std::string name, NodePtr origin, NodePtr destination, RoadType type, Lanes lanes);

        Road(NodePtr origin, NodePtr destination, RoadType type, Lanes lanes);

        [[nodiscard]] const NodePtr &getOrigin() const;

        [[nodiscard]] const NodePtr &getDestination() const;

        [[nodiscard]] const std::optional<std::string> &getName() const;

        [[nodiscard]] RoadType getType() const;

        [[nodiscard]] const Lanes &getLanes() const;

        void setName(const std::optional<std::string> &newName);

        void setType(RoadType newType);

        void setLanes(const Lanes &newLanes);

    private:
        std::optional<std::string> name;
        NodePtr origin;
        NodePtr destination;
        RoadType type;
        Lanes lanes;
    };

    using RoadPtr = std::shared_ptr<Road>;
} // map

#endif //CITYY_ROAD_H
