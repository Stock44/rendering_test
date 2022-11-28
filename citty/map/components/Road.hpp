//
// Created by hiram on 6/27/22.
//

#ifndef CITYY_ROAD_HPP
#define CITYY_ROAD_HPP

#include <optional>
#include <citty/map/RoadType.hpp>

namespace map {

    struct Road {
        std::optional<std::string> name;
        float roadWidth = 2.0f;
        engine::Entity origin;
        engine::Entity destination;
        int lanes = 1;
        int revLanes = 0;
        RoadType type = RoadType::RESIDENTIAL;
    };

} // map

#endif //CITYY_ROAD_HPP
