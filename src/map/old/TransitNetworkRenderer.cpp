//
// Created by hiram on 5/19/22.
//

#include "TransitNetworkRenderer.h"
#include <memory>

namespace map {
    TransitNetworkRenderer::TransitNetworkRenderer(TransitNetwork &network) : network(
            network) {
        static const float laneWidth = 2.75;

        auto highways = network.getHighways();
        for (auto &highwayRecord: highways) {
            // Read the highway record
            auto highwayID = highwayRecord.first;
            auto highway = highwayRecord.second;

            auto startNode = highway->getOrigin();
            auto endNode = highway->getDestination();
            auto color = glm::vec4(1.0f);

            // Switch for selecting way color
            switch (highway->getType()) {
                case map::UNKNOWN:
                    color = glm::vec4(1.0f);
                    break;
                case map::MOTORWAY:
                    color = glm::vec4(233.0 / 255.0, 144.0 / 255.0, 160.0 / 255.0, 1.0f);
                    break;
                case map::TRUNK:
                    color = glm::vec4(251.0 / 255.0, 192.0 / 255.0, 172.0 / 255.0, 1.0f);
                    break;
                case map::PRIMARY:
                    color = glm::vec4(253.0 / 255.0, 215.0 / 255.0, 161.0 / 255.0, 1.0f);
                    break;
                case map::SECONDARY:
                    color = glm::vec4(246.0 / 255.0, 250.0 / 255.0, 187.0 / 255.0, 1.0f);
                    break;
                case map::RESIDENTIAL:
                    color = glm::vec4(1.0f);
                    break;
                case map::LINK:
                    color = glm::vec4(0.8f);
                    break;
                default:
                    break;
            }

            auto [forwardLanes, backwardLanes] = highway->getLanes();
            auto totalLanes = forwardLanes + backwardLanes;

            auto xDistance = endNode->getPosition().x - startNode->getPosition().x;
            auto zDistance = endNode->getPosition().z - startNode->getPosition().z;
            auto distance = sqrt(pow(xDistance, 2) + pow(zDistance, 2));

            auto roadPosition = glm::vec3(startNode->getPosition().x + xDistance / 2.0f, 0.0f,
                                          startNode->getPosition().z + zDistance / 2.0f);
            auto roadRotation = std::atan2(xDistance, zDistance) + std::numbers::pi / 2.0f;

            for (int lane = 0; lane < totalLanes + 1; lane++) {
                auto laneOffsetDistance = static_cast<float>(lane) * laneWidth - (laneWidth * totalLanes) / 2;
                auto laneOffset = glm::vec3(laneOffsetDistance * sin(roadRotation), 0.1f,
                                            laneOffsetDistance * cos(roadRotation));
                auto newLaneSeparator = std::make_shared<graphics::Drawable>(road, glm::vec4(0.0f),
                                                                             roadPosition + laneOffset, roadRotation,
                                                                             glm::vec3(0.0f, 1.0f, 0.0f),
                                                                             glm::vec3(distance, 1.0f, 0.1f));

                objects.push_back(newLaneSeparator);
            }

            auto newObject = std::make_shared<graphics::Drawable>(road, color,
                                                                  roadPosition,
                                                                  roadRotation,
                                                                  glm::vec3(0.0f, 1.0f, 0.0f),
                                                                  glm::vec3(distance, 5.0f, totalLanes * laneWidth));


            objects.push_back(newObject);
        }
    }

    void TransitNetworkRenderer::render(GraphicsEngine &graphics) {
        for (auto object: objects) graphics.draw(object);
    }
} // map