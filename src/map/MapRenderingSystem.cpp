//
// Created by hiram on 7/1/22.
//

#define GLM_FORCE_RADIANS

#include <iostream>
#include "MapRenderingSystem.h"
#include "../graphics/components/MeshRef.h"

namespace map {
    void MapRenderingSystem::setup(engine::ComponentManager &componentManager) {
        roadStore = componentManager.getComponentStore<Road>();
        nodeStore = componentManager.getComponentStore<Node>();
        transformStore = componentManager.getComponentStore<Transform>();
        colorStore = componentManager.getComponentStore<graphics::Color>();
        meshStore = componentManager.getComponentStore<graphics::MeshRef>();

        roadStore->onComponentCreation([this](engine::EntitySet entities) { onRoadCreate(entities); });
        nodeStore->onComponentCreation([this](engine::EntitySet entities) { onNodeCreate(entities); });
        transformStore->onComponentCreation([this](engine::EntitySet entities) { onTransformCreate(entities); });
    }

    void MapRenderingSystem::update(engine::EntityManager &elementManager) {
        // This system doesn't need to periodically update itself, reacts only to events.
    }

    void MapRenderingSystem::onRoadCreate(engine::EntitySet entities) {
        for (auto const &entity: entities) {
            // Skip entity if it doesn't have a road
            if (!roadStore->hasComponent(entity)) continue;

            // Get road endpoints
            auto road = roadStore->getComponent(entity);
            auto originPosition = transformStore->getComponent(road.origin).position;
            auto destinationPosition = transformStore->getComponent(road.destination).position;

            // Calculate road displacement across x and y
            auto xDelta = destinationPosition.x - originPosition.x;
            auto yDelta = destinationPosition.y - originPosition.y;

            auto transform = Transform();
            transform.position = glm::vec3(originPosition.x + xDelta / 2.0f, originPosition.y + yDelta / 2.0f, 0.0f);
            transform.rotation = glm::angleAxis(std::atan2(yDelta, xDelta), glm::vec3{0.0f, 0.0f, 1.0f});
            transform.scale = {std::sqrt(std::pow(xDelta, 2) + std::pow(yDelta, 2)), 1.0f, 1.0f};

            auto color = glm::vec4(1.0f);

            // Switch for selecting way color
            switch (road.type) {
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
                    color = glm::vec4(0.5f);
                    break;
                default:
                    break;
            }

            transformStore->setComponent(entity, transform);
            colorStore->setComponent(entity, color);
            meshStore->setComponent(entity, std::reference_wrapper(cubeMesh));
        }
    }

    void MapRenderingSystem::onNodeCreate(engine::EntitySet entities) {
        for (auto entity: entities) {
            auto transform = transformStore->getComponent(entity);
            transform.scale = {2.0f, 2.0f, 2.0f};
            transformStore->setComponent(entity, transform);
            colorStore->setComponent(entity, graphics::Color{0.9f, 0.4f, 0.4f, 1.0f});
            meshStore->setComponent(entity, std::reference_wrapper(cubeMesh));
        }
    }

    void MapRenderingSystem::onTransformCreate(engine::EntitySet entities) {

    }
} // map