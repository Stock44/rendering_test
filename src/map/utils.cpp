//
// Created by hiram on 5/17/22.
//

#include "utils.h"
#include <cmath>
#include <map>
#include <functional>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <pugixml.hpp>
#include "../engine/ComponentManager.h"
#include "../engine/EntityManager.h"
#include "../components/Transform.h"
#include "components/Road.h"
#include "components/Node.h"

namespace map {
    void handlePugiStatus(pugi::xml_parse_status status) {
        if (status != pugi::xml_parse_status::status_ok) throw std::runtime_error("Error reading XML file");
    }

    RoadType convertHighwayStringToRoadType(std::string_view highwayString) {
        static const std::unordered_map<std::string_view, RoadType> osmHighwayToType = {
                {"road",           UNKNOWN},
                {"motorway",       MOTORWAY},
                {"trunk",          TRUNK},
                {"primary",        PRIMARY},
                {"secondary",      SECONDARY},
                {"tertiary",       TERTIARY},
                {"unclassified",   QUATERNARY},
                {"residential",    RESIDENTIAL},
                {"motorway_link",  LINK},
                {"trunk_link",     LINK},
                {"primary_link",   LINK},
                {"secondary_link", LINK},
                {"tertiary_link",  LINK},
        };
        return osmHighwayToType.at(highwayString);
    }

    // Calculate distance between latitude, longitude pairs
    float distanceLatLon(std::pair<float, float> c1, std::pair<float, float> c2) {
        using std::sin;
        using std::cos;
        using std::sqrt;
        using std::atan2;

        static float radius = 6371000.0f;
        static float rad = std::numbers::pi_v<float> / 180.0f;
        float dLat = c2.first - c1.first;
        float dLon = c2.second - c1.second;
        float a = std::sin(dLat * rad / 2.0f) * std::sin(dLat * rad / 2.0f) +
                  std::cos(c1.first * rad) * std::cos(c2.first * rad) *
                  std::sin(dLon * rad / 2.0f) * std::sin(dLon * rad / 2.0f);
        float c = 2.0f * std::atan2(std::sqrt(a), std::sqrt(1.0f - a));
        return radius * c; // in meters
    }

    void loadXMLMap(const std::string &filePath, engine::ComponentManager &componentManager,
                    engine::EntityManager &entityManager) {


        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filePath.c_str());

        handlePugiStatus(result.status);

        auto nodeStore = componentManager.getComponentStore<Node>();
        auto transformStore = componentManager.getComponentStore<Transform>();
        auto roadStore = componentManager.getComponentStore<Road>();

        // Maps OSM id's to specific components.
        std::unordered_map<std::string_view, engine::Entity> nodes;

        std::pair<double, double> latBounds;
        std::pair<double, double> lonBounds;

        double latMidpoint = 0;
        double latRange = 0;
        double lonMidpoint = 0;
        double lonRange = 0;

        auto rootNode = doc.child("osm");

        // TODO implement custom errors
        if (!rootNode) throw std::runtime_error("Invalid XML File. Missing OSM root node.");

        auto boundsNode = rootNode.child("bounds");

        if (!boundsNode) throw std::runtime_error("Invalid XML File. Missing map bounds node.");

        auto minlatAttribute = boundsNode.attribute("minlat");
        auto maxlatAttribute = boundsNode.attribute("maxlat");
        auto minlonAttribute = boundsNode.attribute("minlon");
        auto maxlonAttribute = boundsNode.attribute("maxlon");

        if (!minlatAttribute || !maxlatAttribute || !minlonAttribute || !maxlonAttribute)
            throw std::runtime_error("Invalid XML File. Malformed bounds node.");

        latBounds = {std::strtod(minlatAttribute.value(), nullptr),
                     std::strtod(maxlatAttribute.value(), nullptr)};
        lonBounds = {std::strtod(minlonAttribute.value(), nullptr),
                     std::strtod(maxlonAttribute.value(), nullptr)};
        latRange = latBounds.second - latBounds.first;
        lonRange = lonBounds.second - lonBounds.first;
        latMidpoint = latBounds.second - latRange / 2.0;
        lonMidpoint = lonBounds.second - lonRange / 2.0;


        // Iterate over all xml nodes and add map nodes
        for (auto node = rootNode.first_child(); node; node = node.next_sibling()) {
            // If xml node is not of OSM type node, don't process it
            if (std::strcmp(node.name(), "node") != 0) continue;

            auto idAttribute = node.attribute("id");
            auto latAttribute = node.attribute("lat");
            auto lonAttribute = node.attribute("lon");

            // If node doesn't contain a necessary attribute, skip it.
            if (!idAttribute || !latAttribute || !lonAttribute) {
                std::cout << "Warning: malformed node ";
                if (idAttribute) std::cout << idAttribute.value();
                std::cout << std::endl;
                continue;
            }

            auto id = idAttribute.value();

            // If node lat and lon values are not numbers, skip the node
            float lat;
            float lon;
            try {
                lat = std::stof(latAttribute.value());
                lon = std::stof(lonAttribute.value());
            } catch (std::invalid_argument const &) {
                std::cout << "Warning: malformed node coordinates " << id << std::endl;
                continue;
            }

            // Generate a new entity for this node
            auto entity = entityManager.createEntity();
            // Create a new node component
            auto nodeComponent = Node();

            // Assign the newly created node to the new entity
            nodeStore->setComponent(entity, nodeComponent);

            // Add the entity to the OSM id to Entity map
            nodes.try_emplace(id, entity);

            // Create the transform for this node
            auto transform = Transform();
            auto &x = transform.position.x;
            auto &y = transform.position.y;
            x = distanceLatLon({0, lon - lonMidpoint}, {0, 0});
            y = distanceLatLon({lat - latMidpoint, 0}, {0, 0});
            x = lon - lonMidpoint > 0 ? x : -x;
            y = lat - latMidpoint > 0 ? y : -y;

            // Assign the transform for the node's entity
            transformStore->setComponent(entity, transform);

        }

        // Iterate over all xml nodes and add map roads
        for (auto node = rootNode.first_child(); node; node = node.next_sibling()) {

            // If xml node is not a way, skip it
            if (std::strcmp(node.name(), "way") != 0) continue;

            // Find the highway tag
            auto highwayTag = node.find_child([](pugi::xml_node subnode) {
                auto keyAttribute = subnode.attribute("k");
                return keyAttribute && std::strcmp(keyAttribute.value(), "highway") == 0;
            });

            // If the highway tag was not found, skip this way
            if (!highwayTag) continue;

            RoadType roadType;

            try {
                roadType = convertHighwayStringToRoadType(highwayTag.attribute("v").value());
            } catch (std::out_of_range const &) {
                continue;
            }

            int lanes = 0;
            int revLanes = 0;

            // Find the lanes tag
            auto lanesTag = node.find_child([](pugi::xml_node subnode) {
                auto keyAttribute = subnode.attribute("k");
                return keyAttribute && std::strcmp(keyAttribute.value(), "lanes") == 0;
            });

            // Deduce lane configuration for this way
            if (lanesTag) {
                int totalLanes = std::stoi(lanesTag.attribute("v").value(), nullptr);

                // Find the lanes tag
                auto onewayTag = node.find_child([](pugi::xml_node subnode) {
                    auto keyAttribute = subnode.attribute("k");
                    return keyAttribute && std::strcmp(keyAttribute.value(), "oneway") == 0;
                });

                if (onewayTag) {
                    if (std::strcmp(onewayTag.attribute("v").value(), "yes") == 0) {
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
                if (roadType == LINK) {
                    lanes = 1;
                } else {
                    lanes = 1;
                    revLanes = 1;
                }
            }

            // Create a road for each node of type nd within the Way node
            for(auto subnode = node.first_child(); subnode.next_sibling(); subnode = subnode.next_sibling()) {
                auto nextSubnode = subnode.next_sibling();
                if (std::strcmp(subnode.name(), "nd") != 0 && std::strcmp(nextSubnode.name(), "nd") != 0) continue;
                auto originNodeIDAttr = subnode.attribute("ref");
                auto destinationNodeIDAttr = nextSubnode.attribute("ref");
                // If this subnode or the next subnode lack a ref attribute, skip this pair.
                if (!originNodeIDAttr || !destinationNodeIDAttr) {
                    subnode = nextSubnode;
                    continue;
                }

                engine::Entity originNodeEntity;
                engine::Entity destinationNodeEntity;

                // If this road's nodes are not on the map, skip creating this road
                try {
                    originNodeEntity = nodes.at(originNodeIDAttr.value());
                    destinationNodeEntity = nodes.at(destinationNodeIDAttr.value());
                } catch (std::out_of_range const &) {
                    continue;
                }



                Road road;
                road.origin = originNodeEntity;
                road.destination = destinationNodeEntity;
                road.lanes = lanes;
                road.revLanes = revLanes;
                road.type = roadType;

                auto roadEntity = entityManager.createEntity();
                roadStore->setComponent(roadEntity, road);

                auto originNodeComponent = nodeStore->getComponent(originNodeEntity);
                originNodeComponent.parentRoads.push_back(roadEntity);
                nodeStore->setComponent(originNodeEntity, std::move(originNodeComponent));

                auto destinationNodeComponent = nodeStore->getComponent(destinationNodeEntity);
                destinationNodeComponent.parentRoads.push_back(roadEntity);
                nodeStore->setComponent(destinationNodeEntity, std::move(destinationNodeComponent));

            }
        }

        std::map addedNodes = nodeStore->getComponents();

        for (auto const &[entity, node] : addedNodes) {
            if (node.parentRoads.empty()) nodeStore->deleteComponent(entity);
        }

    }


} // map