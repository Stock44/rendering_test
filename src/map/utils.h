//
// Created by hiram on 5/17/22.
//
#ifndef XML_UTILS
#define XML_UTILS

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
#include "../engine/ComponentManager.h"
#include "../engine/EntityManager.h"
#include "../components/Transform.h"
#include "components/Road.h"
#include "components/Node.h"

namespace map {
    void handlePugiStatus(pugi::xml_parse_status status);

    RoadType convertHighwayStringToRoadType(std::string_view highwayString);

    // Calculate distance between latitude, longitude pairs
    float distanceLatLon(std::pair<float, float> c1, std::pair<float, float> c2);

    void loadXMLMap(const std::string &filePath, engine::ComponentManager &componentManager,
                    engine::EntityManager &entityManager);
}

#endif