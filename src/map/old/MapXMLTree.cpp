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
#include <algorithm>
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
        using node = xml_node<>;

        auto osmRoot = XMLTree.first_node();


        std::pair<double, double> latBounds, lonBounds;

        double latMidpoint = 0;
        double latRange = 0;
        double lonMidpoint = 0;
        double lonRange = 0;

        map::HighwayMap roads;
        map::NodeMap nodes;
        std::map<std::string_view, int> osmIDMap;

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

                osmIDMap[nodeAttributes.at("id")] = nodes.size();
                int id = nodes.size();
                auto lat = std::strtod(&nodeAttributes.at("lat")[0], nullptr);
                auto lon = std::strtod(&nodeAttributes.at("lon")[0], nullptr);
                auto x = distanceLatLon({0, lon - lonMidpoint}, {0, 0});
                auto y = distanceLatLon({lat - latMidpoint, 0}, {0, 0});
                x = lon - lonMidpoint > 0 ? -x : x;
                y = lat - latMidpoint > 0 ? y : -y;
                nodes.try_emplace(id, std::make_shared<Node>(nodes.size(), glm::vec3(x, 0.0f, y)));
            } else if (elementName == "way") {
                auto wayTags = getTags<char>(element);
                // If this way is a highway
                if (wayTags.contains("highway")) {
                    auto highwayString = std::string(wayTags.at("highway"));
                    // If it is of a supported highway type
                    if (osmHighwayToType.contains(highwayString)){
                        auto memberNodesIDs = getWayMemberNodesIDs<char>(element);
                        auto type = osmHighwayToType.at(highwayString);

                        int lanes = 0;
                        int revLanes = 0;

                        if(wayTags.contains("lanes")){
                            int totalLanes = std::stoi(&wayTags.at("lanes")[0], nullptr);

                            if(wayTags.contains("oneway")) {
                                if(wayTags.at("oneway") == "yes") {
                                    lanes = totalLanes;
                                } else {
                                    lanes = totalLanes / 2;
                                    revLanes = totalLanes - lanes;
                                }
                            } else {
                                lanes = totalLanes / 2;
                                revLanes = totalLanes - lanes;
                            }
                        } else {
                            if (type == LINK) {
                                lanes = 1;
                            } else {
                                lanes = 1;
                                revLanes = 1;
                            }
                        }

                        for (auto it = memberNodesIDs.begin(); it < memberNodesIDs.end() - 1; it++) {
                            std::shared_ptr<Road> newRoad;
                            auto originNode = nodes.at(osmIDMap.at(*it));
                            auto destinationNode = nodes.at(osmIDMap.at(*(it + 1)));
                            if (wayTags.contains("name")) {
                                newRoad = std::make_shared<Road>(std::string(wayTags.at("name")), originNode, destinationNode, type, std::make_pair(lanes, revLanes));
                            } else {
                                newRoad = std::make_shared<Road>(originNode, destinationNode, type, std::make_pair(lanes, revLanes));
                            }
                            originNode->addParentRoad(std::weak_ptr(newRoad));
                            destinationNode->addParentRoad(std::weak_ptr(newRoad));
                            roads.try_emplace(roads.size(), newRoad);
                        }
                    }
                }
            }
        });
        NodeMap finalNodes;
        std::ranges::for_each(nodes, [&finalNodes] (std::pair<int, NodePtr> node) {
            if(!node.second->getParentRoads().empty()) finalNodes.insert(finalNodes.end(), node);
        });
        return {finalNodes, roads};
    }

} // map