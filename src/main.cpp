#include <chrono>
#include <iostream>
#include <thread>
#include "Window.h"
#include "engine/Engine.h"
#include "graphics/RenderingSystem.h"
#include "input/InputSystem.h"
#include "map/loadXMLMap.h"
#include "map/MapRenderingSystem.h"

const graphics::Mesh cubeMesh = {
        0,
        {
                graphics::Vertex(0.5f, 0.5f, 0.5f),
                graphics::Vertex(-0.5f, 0.5f, 0.5f),
                graphics::Vertex(0.5f, 0.5f, -0.5f),
                graphics::Vertex(-0.5f, 0.5f, -0.5f),
                graphics::Vertex(0.5f, -0.5f, 0.5f),
                graphics::Vertex(-0.5f, -0.5f, 0.5f),
                graphics::Vertex(0.5f, -0.5f, -0.5f),
                graphics::Vertex(-0.5f, -0.5f, -0.5f),
        },
        {
                1, 0, 2,
                2, 3, 1,
                4, 6, 2,
                2, 0, 4,
                5, 4, 0,
                0, 1, 5,
                7, 5, 1,
                1, 3, 7,
                7, 6, 4,
                4, 5, 7,
                6, 7, 2,
                2, 3, 7,
        },
};

int main() {
    Window window({500, 500});

    engine::Engine engine;

    engine.registerSystem(std::make_unique<graphics::RenderingSystem>(window));
    engine.registerSystem(std::make_unique<input::InputSystem>(window));
    engine.registerSystem(std::make_unique<map::MapRenderingSystem>());

    auto &entityManager = engine.getEntityManager();
    auto &componentManager = engine.getComponentManager();

    auto transformStore = componentManager.getComponentStore<Transform>();
    auto colorStore = componentManager.getComponentStore<graphics::Color>();
    auto meshStore = componentManager.getComponentStore<graphics::MeshRef>();
    auto cameraStore = componentManager.getComponentStore<graphics::Camera>();

    auto entity = entityManager.createEntity();
    auto entity2 = entityManager.createEntity();
    auto entity3 = entityManager.createEntity();
    auto cameraEntity = entityManager.createEntity();

    auto entityTransform = Transform();
    entityTransform.position = {20.0f, 0.0f, 0.0f};
    entityTransform.scale = {5.0f, 5.0f, 5.0f};


    auto entity2Transform = entityTransform;
    entity2Transform.position = {-20.0f, 0.0f, 0.0f};
    entity2Transform.scale = {10.0f, 10.0f, 10.0f};

    auto entity3Transform = entityTransform;
    entity3Transform.position = {0.0f, 20.0f, 0.0f};
    entity3Transform.scale = {20.0f, 20.0f, 20.0f};

    transformStore->setComponent(entity, entityTransform);
    colorStore->setComponent(entity, graphics::Color{1.0f, 0.0f, 0.0f, 1.0f});
    meshStore->setComponent(entity, graphics::MeshRef{cubeMesh});

    transformStore->setComponent(entity2, entity2Transform);
    colorStore->setComponent(entity2, graphics::Color{0.0f, 1.0f, 0.0f, 1.0f});
    meshStore->setComponent(entity2, graphics::MeshRef{cubeMesh});

    transformStore->setComponent(entity3, entity3Transform);
    colorStore->setComponent(entity3, graphics::Color{0.0f, 0.0f, 1.0f, 1.0f});
    meshStore->setComponent(entity3, graphics::MeshRef{cubeMesh});

    auto cameraTransform = Transform();
    transformStore->setComponent(cameraEntity, cameraTransform);
    auto cameraComponent = graphics::Camera();
    cameraStore->setComponent(cameraEntity, cameraComponent);

    map::loadXMLMap("/home/hiram/Projects/citty/samples/sample_map.osm", componentManager, entityManager);
    std::cout << "Number of entities in the system: " << entityManager.createEntity() << std::endl;

    while (!window.shouldWindowClose()) {
        auto last = std::chrono::steady_clock::now();


        engine.update();

        auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - last);
        std::cout << "FPS: " << 1000000000.f / delta.count() << std::endl;
//        if (delta.count() > 1.0f / 120.0f) {
//            std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(delta.count() - 1000.0f / 120.0f));
//        }
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
