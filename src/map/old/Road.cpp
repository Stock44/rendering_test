//
// Created by hiram on 5/17/22.
//

#include "Road.h"

namespace map {

    Road::Road(std::string name, NodePtr origin, NodePtr destination, RoadType type, Lanes lanes) : name(name),
                                                                                                    origin(origin),
                                                                                                    destination(
                                                                                                            destination),
                                                                                                    type(type),
                                                                                                    lanes(std::move(lanes)) {}

    Road::Road(NodePtr origin, NodePtr destination, RoadType type, Lanes lanes) : origin(origin),
                                                                                  destination(destination),
                                                                                  type(type),
                                                                                  lanes(std::move(lanes)) {}

    const NodePtr &Road::getOrigin() const {
        return origin;
    }

    const NodePtr &Road::getDestination() const {
        return destination;
    }

    const std::optional<std::string> &Road::getName() const {
        return name;
    }

    void Road::setName(const std::optional<std::string> &newName) {
        Road::name = newName;
    }

    RoadType Road::getType() const {
        return type;
    }

    void Road::setType(RoadType newType) {
        Road::type = newType;
    }

    const Lanes &Road::getLanes() const {
        return lanes;
    }

    void Road::setLanes(const Lanes &newLanes) {
        Road::lanes = newLanes;
    }
} // map