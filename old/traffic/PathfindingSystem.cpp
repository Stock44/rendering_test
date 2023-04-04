//
// Created by hiram on 7/6/22.
//

#include <citty/traffic/PathfindingSystem.h>
#include <typeinfo>

namespace traffic {
    void PathfindingSystem::setup(citty::ComponentManager &componentManager) {
        transformStore = componentManager.getComponentStore<Transform>();
        roadStore = componentManager.getComponentStore<map::Road>();
        nodeStore = componentManager.getComponentStore<map::Node>();
        targetStore = componentManager.getComponentStore<PathfindTarget>();
        pathStore = componentManager.getComponentStore<Path>();

        roadStore->onComponentCreation([this](citty::EntitySet entities) { onRoadCreate(entities); });
        targetStore->onComponentCreation([this](citty::EntitySet entities) { onTargetCreate(entities); });
        transformStore->onComponentCreation([this](citty::EntitySet entities) { onTransformCreate(entities); });
    }

    void PathfindingSystem::tryRegisterRoad(citty::Entity entity) {
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

//        nodeTree.insert(std::make_pair(box, entity));

        auto v1 = boost::add_vertex(road.origin, roadGraph);
        auto v2 = boost::add_vertex(road.destination, roadGraph);

        boost::add_edge(v1, v2, PathProperties{entity, calculateRoadCost(roadStore->getComponent(entity))}, roadGraph);

        loadedRoads.emplace(entity);
    }

    void PathfindingSystem::update(citty::EntityManager &elementManager) {

    }

    void PathfindingSystem::onRoadCreate(citty::EntitySet entities) {
        for (auto entity: entities) {
            tryRegisterRoad(entity);
        }
    }

    void PathfindingSystem::onTransformCreate(citty::EntitySet entities) {
        for (auto entity: entities) {
            tryRegisterRoad(entity);
        }
    }

    void PathfindingSystem::onTargetCreate(citty::EntitySet entities) {
        using boost::geometry::index::nearest;
        using boost::geometry::distance;

        // Create a path for each target and delete the target.
        for (auto entity: entities) {
            auto targetPosition = targetStore->getComponent(entity).position;
            auto originPosition = transformStore->getComponent(entity).position;

            std::vector<BoxRoadPair> foundRoads;
//            nodeTree.query(nearest(originPosition, 1), std::back_inserter(foundRoads));
            auto originRoad = roadStore->getComponent(foundRoads.at(0).second);
            foundRoads.clear();
//            nodeTree.query(nearest(targetPosition, 1),
//                           std::back_inserter(foundRoads));
            auto destinationRoad = roadStore->getComponent(foundRoads.at(0).second);

            auto originNodePosition = transformStore->getComponent(originRoad.origin).position;
            auto destinationNodePosition = transformStore->getComponent(originRoad.destination).position;
            auto originNode = originRoad.origin;
            if (distance(originPosition, originNodePosition) > distance(originPosition, destinationNodePosition)) {
                originNode = originRoad.destination;
            }

            originNodePosition = transformStore->getComponent(destinationRoad.origin).position;
            destinationNodePosition = transformStore->getComponent(destinationRoad.destination).position;
            auto destinationNode = destinationRoad.origin;
            if (distance(targetPosition, originNodePosition) > distance(targetPosition, destinationNodePosition)) {
                destinationNode = destinationRoad.destination;
            }

//            RoadGraph::vertex_descriptor originVertex;
//            RoadGraph::vertex_descriptor destinationVertex;

//            for (auto vertex : boost::make_iterator_range(boost::vertices(roadGraph))) {
//                if (roadGraph[vertex] == originNode) {
//                    originVertex = vertex;
//                } else if (roadGraph[vertex] == destinationNode) {
//                    destinationVertex = vertex;
//                }
//            }

//            boost::astar_search_tree(roadGraph, originNode);

            // TODO what if the path is not found?
//            if (foundPath) pathStore->setComponent(entity, foundPath.value());

//            targetStore->deleteComponent(entity);
        }
    }

    std::optional<Path>
    PathfindingSystem::findPath(citty::Entity originRoadEntity, citty::Entity destinationRoadEntity) {
        return {};
    }

    float PathfindingSystem::calculateRoadCost(map::Road const &road) const {
        auto origin = transformStore->getComponent(road.origin).position;
        auto destination = transformStore->getComponent(road.destination).position;

        float cost = boost::geometry::distance(origin, destination);
        switch (road.type) {
            case map::UNKNOWN:
                cost *= 2.f;
                break;
            case map::MOTORWAY:
                cost *= 1.1f;
                break;
            case map::PRIMARY:
                cost *= 1.2f;
                break;
            case map::SECONDARY:
                cost *= 1.4f;
                break;
            case map::TERTIARY:
                cost *= 1.6f;
                break;
            case map::QUATERNARY:
                cost *= 1.8f;
                break;
            case map::LINK:
            case map::RESIDENTIAL:
                cost *= 2.f;
                break;
            case map::TRUNK:
            default:
                break;
        }

        return cost;
    }
} // traffic