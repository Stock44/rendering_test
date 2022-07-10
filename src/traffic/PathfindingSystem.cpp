//
// Created by hiram on 7/6/22.
//

#include "PathfindingSystem.h"


namespace traffic {
    void PathfindingSystem::setup(engine::ComponentManager &componentManager) {
        transformStore = componentManager.getComponentStore<Transform>();
        roadStore = componentManager.getComponentStore<map::Road>();
        nodeStore = componentManager.getComponentStore<map::Node>();
        targetStore = componentManager.getComponentStore<PathfindTarget>();
        pathStore = componentManager.getComponentStore<Path>();

        roadStore->onComponentCreation([this](engine::EntitySet entities) { onRoadCreate(entities); });
        targetStore->onComponentCreation([this](engine::EntitySet entities) { onTargetCreate(entities); });
        transformStore->onComponentCreation([this](engine::EntitySet entities) { onTransformCreate(entities); });
    }

    void PathfindingSystem::tryRegisterRoad(engine::Entity entity) {
        // If an entity doesn't have either a road or a transform, don't add it
        if (!roadStore->hasComponent(entity)) return;
        if (!transformStore->hasComponent(entity)) return;

        // If this road has already been loaded, skip it
        if (loadedRoads.contains(entity)) return;

        auto road = roadStore->getComponent(entity);

        auto originPosition = transformStore->getComponent(road.origin).position;
        auto destinationPosition = transformStore->getComponent(road.destination).position;

        // Generate box and create its minimum and maximum corners based on road nodes' positions
        auto box = Box();
        if (originPosition.x < destinationPosition.x) {
            box.min_corner().x = originPosition.x;
            box.max_corner().x = destinationPosition.x;
        } else {
            box.min_corner().x = destinationPosition.x;
            box.max_corner().x = originPosition.x;
        }

        if (originPosition.y < destinationPosition.y) {
            box.min_corner().y = originPosition.y;
            box.max_corner().y = destinationPosition.y;
        } else {
            box.min_corner().y = destinationPosition.y;
            box.max_corner().y = originPosition.y;
        }

        if (originPosition.z < destinationPosition.z) {
            box.min_corner().z = originPosition.z;
            box.max_corner().z = destinationPosition.z;
        } else {
            box.min_corner().z = destinationPosition.z;
            box.max_corner().z = originPosition.z;
        }

        roadTree.insert(std::make_pair(box, entity));
        loadedRoads.emplace(entity);
    }

    void PathfindingSystem::update(engine::EntityManager &elementManager) {

    }

    void PathfindingSystem::onRoadCreate(engine::EntitySet entities) {
        for (auto entity: entities) {
            tryRegisterRoad(entity);
        }
    }

    void PathfindingSystem::onTransformCreate(engine::EntitySet entities) {
        for (auto entity: entities) {
            tryRegisterRoad(entity);
        }
    }

    void PathfindingSystem::onTargetCreate(engine::EntitySet entities) {
        // Create a path for each target and delete the target.
        for (auto entity: entities) {
            auto originPosition = transformStore->getComponent(entity).position;
            std::cout << "Searching " << roadTree.size() << " roads" << std::endl;
            std::vector<BoxPair> results;
            roadTree.query(boost::geometry::index::nearest(originPosition, 5), std::back_inserter(results));
            std::cout << "Closest entity to origin position: " << results.at(0).second << std::endl;
        }
    }
} // traffic