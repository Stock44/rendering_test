//
// Created by hiram on 5/17/22.
//

#include "Highway.h"

namespace map {
    HighwayType Highway::getType() const {
        return type;
    }

    void Highway::setType(HighwayType type) {
        Highway::type = type;
    }

    Highway::Highway(std::string name, HighwayType type, Lanes lanes) : name(name), type(type), lanes(lanes) {

    }

    const Lanes &Highway::getLanes() const {
        return lanes;
    }

    void Highway::setLanes(const Lanes &lanes) {
        Highway::lanes = lanes;
    }

    const std::string &Highway::getName() const {
        return name.value();
    }

    void Highway::setName(const std::string &name) {
        Highway::name = name;
    }

    Highway::Highway(HighwayType type, Lanes lanes) : type(type), lanes(lanes) {

    }

    bool Highway::hasName() {
        return name.has_value();
    }
} // map