#include <chrono>
#include <iostream>
#include <thread>
#include "Window.h"
#include "engine/Engine.h"
#include "graphics/RenderingSystem.h"
#include "input/InputSystem.h"
#include "map/loadXMLMap.h"
#include "map/MapRenderingSystem.h"
#include "traffic/components/Target.h"
#include "traffic/PathfindingSystem.h"

int main() {
    using namespace std::chrono;
    Window window({500, 500});

    engine::Engine engine;

    engine.registerSystem(std::make_unique<graphics::RenderingSystem>(window));
    engine.registerSystem(std::make_unique<input::InputSystem>(window));
    engine.registerSystem(std::make_unique<map::MapRenderingSystem>());
    engine.registerSystem(std::make_unique<traffic::PathfindingSystem>());

    auto &entityManager = engine.getEntityManager();
    auto &componentManager = engine.getComponentManager();

    auto transformStore = componentManager.getComponentStore<Transform>();
    auto cameraStore = componentManager.getComponentStore<graphics::Camera>();
    auto targetStore = componentManager.getComponentStore<traffic::PathfindTarget>();

    auto cameraEntity = entityManager.createEntity();

    auto cameraTransform = Transform();
    transformStore->setComponent(cameraEntity, cameraTransform);
    auto cameraComponent = graphics::Camera();
    cameraStore->setComponent(cameraEntity, cameraComponent);

    map::loadXMLMap("/home/hiram/Projects/citty/samples/sample_map.osm", componentManager, entityManager);
    std::cout << "Number of entities in the system: " << entityManager.createEntity() << std::endl;

    while (!window.shouldWindowClose()) {
        auto last = std::chrono::steady_clock::now();


        engine.update();


        auto delta = steady_clock::now() - last;
        std::cout << "Frame duration : " << delta << " FPS: " << 1000000000.0 / delta.count()  << std::endl;
        if (delta.count() < 1.0 / 120000000000.0) {
            std::this_thread::sleep_for(nanoseconds(1000000000 / 120) - delta);
        }
    }
}


//    // A* algorithm implementation
//    std::unordered_map<int, float> nodeDistances = {{initialNode->getId(), 0}};
//    std::unordered_map<int, float> nodeCosts = {
//            {initialNode->getId(), glm::distance(initialNode->getPosition(), targetNode->getPosition())}};
//    std::unordered_map<int, map::NodePtr> visitedNodes;
//    std::unordered_map<int, map::NodePtr> unvisitedNodes = {{initialNode->getId(), initialNode}};
//    std::unordered_map<int, map::NodePtr> visitedFrom;
//
//    while (true) {
//        map::NodePtr visitorNode;
//        // Find the visited node that has the lowest distance
//        for (auto const &nodeRecord: unvisitedNodes) {
//            // Don't do anything if this node is not closer than the currently selected node.
//            if (!visitorNode) visitorNode = (*unvisitedNodes.begin()).second;
//
//            float currentCompoundValue =
//                    nodeDistances.at(visitorNode->getId()) + nodeCosts.at(visitorNode->getId());
//
//            float recordCompoundValue = nodeDistances.at(nodeRecord.first) + nodeCosts.at(nodeRecord.first);
//
//            if (recordCompoundValue >= currentCompoundValue) continue;
//            visitorNode = nodeRecord.second;
//        }
//
//        if (visitorNode->getId() == targetNode->getId()) break;
//
//        visitedNodes[visitorNode->getId()] = unvisitedNodes.at(visitorNode->getId());
//        unvisitedNodes.erase(visitorNode->getId());
//        // For the selected node, find its neighbors, check if they're registered,
//        for (auto road: visitorNode->getParentRoads()) {
//            auto [forwardLanes, backwardLanes] = std::reinterpret_pointer_cast<map::Road>(road.lock())->getLanes();
//            auto roadOrigin = road.lock()->getOrigin();
//            auto roadEnd = road.lock()->getDestination();
//
//            auto tentativeCost =
//                    nodeDistances.at(visitorNode->getId()) +
//                    glm::distance(visitorNode->getPosition(), roadEnd->getPosition());
//            if (nodeDistances.contains(roadEnd->getId())) {
//                auto assignedCost = nodeDistances.at(roadEnd->getId());
//                if (assignedCost > tentativeCost) {
//                    nodeDistances.at(roadEnd->getId()) = tentativeCost;
//                    visitedFrom.at(roadEnd->getId()) = visitorNode;
//                }
//            } else {
//                nodeDistances[roadEnd->getId()] =
//                        nodeDistances.at(visitorNode->getId()) +
//                        glm::distance(visitorNode->getPosition(), roadEnd->getPosition());
//                nodeCosts[roadEnd->getId()] = glm::distance(roadEnd->getPosition(), targetNode->getPosition());
//                unvisitedNodes[roadEnd->getId()] = roadEnd;
//                visitedFrom[roadEnd->getId()] = visitorNode;
//            }
//        }
//    }
//
