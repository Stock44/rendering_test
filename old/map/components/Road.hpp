//
// Created by hiram on 6/27/22.
//

#pragma once

#include <optional>
#include <citty/map/RoadType.hpp>

namespace map {

    struct Road {
        std::optional<std::string> name;
        float roadWidth = 2.0f;
        citty::Entity origin;
        citty::Entity destination;
        int lanes = 1;
        int revLanes = 0;
        RoadType type = RoadType::RESIDENTIAL;
    };

} // map
