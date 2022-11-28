//
// Created by hiram on 7/1/22.
//

#define GLM_FORCE_RADIANS

#include <iostream>
#include <citty/map/MapRenderingSystem.hpp>
#include <citty/graphics/components/MeshRef.hpp>
#include <citty/graphics/components/Color.hpp>

namespace map {
    void MapRenderingSystem::setup(engine::ComponentManager &componentManager) {
        roadStore = componentManager.getComponentStore<Road>();
        nodeStore = componentManager.getComponentStore<Node>();
        transformStore = componentManager.getComponentStore<Transform>();
        colorStore = componentManager.getComponentStore<graphics::Color>();
        meshStore = componentManager.getComponentStore<graphics::MeshRef>();

        roadStore->onComponentCreation([this](engine::EntitySet entities) { onRoadCreate(entities); });
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
            transform.scale = {std::sqrt(std::pow(xDelta, 2) + std::pow(yDelta, 2)),
                               road.roadWidth * (road.lanes + road.revLanes), 1.0f};

            graphics::Color color;

            // Switch for selecting way color
            switch (road.type) {
                case MOTORWAY:
                    color = graphics::Color(232.0 / 255.0, 146.0 / 255.0, 162.0 / 255.0, 1.0f);
                    break;
                case TRUNK:
                    color = graphics::Color(249.0 / 255.0, 178.0 / 255.0, 156.0 / 255.0, 1.0f);
                    break;
                case PRIMARY:
                    color = graphics::Color(252.0 / 255.0, 214.0 / 255.0, 164.0 / 255.0, 1.0f);
                    break;
                case SECONDARY:
                    color = graphics::Color(247.0 / 255.0, 250.0 / 255.0, 191.0 / 255.0, 1.0f);
                    break;
                case LINK:
                    color = graphics::Color(0.6f);
                    break;
                case UNKNOWN:
                case RESIDENTIAL:
                case TERTIARY:
                case QUATERNARY:
                    color = graphics::Color(1.0f);
                    break;
            }

            transformStore->setComponent(entity, transform);
            colorStore->setComponent(entity, color);
            meshStore->setComponent(entity, std::reference_wrapper(cubeMesh));
        }
    }

    void MapRenderingSystem::onTransformCreate(engine::EntitySet entities) {

    }
} // map