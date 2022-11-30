//
// Created by hiram on 5/17/22.
//

#pragma once

#include <cmath>
#include <map>
#include <functional>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <queue>
#include <iostream>
#include <glm/vec4.hpp>
#include <algorithm>
#include <pugixml.hpp>
#include <citty/engine/ComponentManager.hpp>
#include <citty/engine/EntityManager.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/map/components/Road.hpp>
#include <citty/map/components/Node.hpp>

namespace map {
    void handlePugiStatus(pugi::xml_parse_status status);

    RoadType convertHighwayStringToRoadType(std::string_view highwayString);

    // Calculate distance between latitude, longitude pairs
    float distanceLatLon(std::pair<float, float> c1, std::pair<float, float> c2);

    void loadXMLMap(const std::string &filePath, engine::ComponentManager &componentManager,
                    engine::EntityManager &entityManager);
}