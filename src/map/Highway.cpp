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

    Highway::Highway(HighwayType type) : type(type){

    }
} // map