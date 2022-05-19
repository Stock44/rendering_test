//
// Created by hiram on 5/17/22.
//

#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <queue>
#include <iostream>
#include <glm/vec4.hpp>
#include "MapXMLTree.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "utils.h"

namespace map {
    MapXMLTree::MapXMLTree(const std::string &filePath) : file(filePath.c_str()) {
        XMLTree.parse<0>(file.data());
    }

    TransitNetwork MapXMLTree::generateNetwork() {
        using namespace rapidxml;
        typedef xml_node<> node;

        auto osmRoot = XMLTree.first_node();


        std::pair<double, double> latBounds, lonBounds;

        double latMidpoint = 0;
        double latRange = 0;
        double lonMidpoint = 0;
        double lonRange = 0;

        std::map<std::string_view, int> nodeIDMap;
        map::HighwayMap highways;
        map::NodeMap nodes;

        // Process node block
        processChildNodes<char>(osmRoot, [&](xml_node<> *element) {
            auto elementName = std::string_view(element->name());
            if (elementName == "bounds") {
                auto nodeAttributes = getAttributes<char>(element);
                latBounds = {std::strtod(&nodeAttributes.at("minlat")[0], nullptr),
                             std::strtod(&nodeAttributes.at("maxlat")[0], nullptr)};
                lonBounds = {std::strtod(&nodeAttributes.at("minlon")[0], nullptr),
                             std::strtod(&nodeAttributes.at("maxlon")[0], nullptr)};
                latRange = latBounds.second - latBounds.first;
                lonRange = lonBounds.second - lonBounds.first;
                latMidpoint = latBounds.second - latRange / 2.0;
                lonMidpoint = lonBounds.second - lonRange / 2.0;
            } else if (elementName == "node") {
                auto nodeAttributes = getAttributes<char>(element);

                nodeIDMap[nodeAttributes["id"]] = nodes.size();

                auto lat = std::strtod(&nodeAttributes.at("lat")[0], nullptr);
                auto lon = std::strtod(&nodeAttributes.at("lon")[0], nullptr);
                auto x = distanceLatLon({0, lon - lonMidpoint}, {0, 0});
                auto y = distanceLatLon({lat - latMidpoint, 0}, {0, 0});
                x = lon - lonMidpoint > 0 ? x : -x;
                y = lat - latMidpoint > 0 ? y : -y;
                nodes.insert({nodes.size(), std::make_shared<Node>(glm::vec3(x, 0.0f, y))});
            } else if (elementName == "way") {
                auto wayTags = getTags<char>(element);
                if (wayTags.contains("highway")) {
                    auto highwayValue = std::string(wayTags.at("highway"));
                    if (osmHighwayToType.contains(highwayValue)){
                        auto memberNodesIDs = getWayMemberNodesIDs<char>(element);
                        auto type = osmHighwayToType.at(highwayValue);
                        auto newHighway = std::make_shared<Highway>(type);
                        for (const auto &nodeID: memberNodesIDs) {
                            newHighway->addNode(nodes.at(nodeIDMap.at(nodeID)));
                        }
                        highways.insert({highways.size(), newHighway});
                    }
                }
            }
        });
        return {nodes, highways};
    }

} // map