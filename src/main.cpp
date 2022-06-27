#include <chrono>
#include <iostream>
#include <thread>
#include "engine/ComponentManager.h"
#include "Window.h"
#include "engine/Engine.h"
#include "graphics/RenderingSystem.h"
#include "input/InputSystem.h"

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
    entityTransform.scale = {10.0f, 10.0f, 10.0f};


    auto entity2Transform = entityTransform;
    entity2Transform.position = {-20.0f, 0.0f, 0.0f};
    entity2Transform.scale = {10.0f, 10.0f, 10.0f};

    auto entity3Transform = entityTransform;
    entity3Transform.position = {0.0f, 20.0f, 0.0f};
    entity3Transform.scale = {10.0f, 10.0f, 10.0f};

    transformStore->setComponent(entity, entityTransform);
    colorStore->setComponent(entity, graphics::Color{1.0f, 1.0f, 1.0f, 1.0f});
    meshStore->setComponent(entity, graphics::MeshRef{cubeMesh});

    transformStore->setComponent(entity2, entity2Transform);
    colorStore->setComponent(entity2, graphics::Color{1.0f, 1.0f, 1.0f, 1.0f});
    meshStore->setComponent(entity2, graphics::MeshRef{cubeMesh});

    transformStore->setComponent(entity3, entity3Transform);
    colorStore->setComponent(entity3, graphics::Color{1.0f, 1.0f, 1.0f, 1.0f});
    meshStore->setComponent(entity3, graphics::MeshRef{cubeMesh});

    auto cameraTransform = Transform();
    transformStore->setComponent(cameraEntity, cameraTransform);
    auto cameraComponent = graphics::Camera();
    cameraStore->setComponent(cameraEntity, cameraComponent);

    auto last = std::chrono::steady_clock::time_point();
    while (!window.shouldWindowClose()) {
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last);
        last = std::chrono::steady_clock::now();

//        entityTransform.rotationAngle += 25.0f * delta.count() / 1000.0f;
//        entityTransform.rotationAngle = entityTransform.rotationAngle > 180 ? -180 : entityTransform.rotationAngle;
//        transformStore->setComponent(entity, entityTransform);

        engine.update();

        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(10));
    }
}

//int main2() {
//    // Initialize window, camera and graphics engine.
//    graphics::Camera camera(glm::dvec3(0.0f, 1.0f, 0.0f), 35.0f,
//                            1.0f, 0.0f, 0.0f);
//    auto graphics = graphics::GraphicsEngine(window, camera);
//
//    // Keep track of frame duration
//    auto lastFrame = std::chrono::steady_clock::now(); // Start time of last frame
//    std::chrono::duration<float, std::milli> deltaTime{}; // Duration of last frame in seconds
//    const int FPSLimit = 120;
//    const auto frameDuration = std::chrono::duration<float, std::milli>(1000.0f / static_cast<float>(FPSLimit));
//
//    // Keep track of last known mouse position
//    auto lastMousePos = std::make_pair(0.0f, 0.0f);
//
//    // Input handlers
//    auto handleInput = [&deltaTime, &window, &camera] {
//        const float cameraSpeed = 200.0f * deltaTime.count() / 1000.0f;
//        auto cameraPos = camera.getCameraPos();
//        auto cameraFront = camera.getCameraFront();
//        auto cameraUp = camera.getCameraUp();
//        if (window.getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//            window.close();
//        }
//        if (window.getKeyState(GLFW_KEY_1) == GLFW_PRESS) {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        }
//        if (window.getKeyState(GLFW_KEY_2) == GLFW_PRESS) {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        }
//        if (window.getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
//            cameraPos += cameraSpeed * cameraFront;
//        }
//        if (window.getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
//            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//        }
//        if (window.getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
//            cameraPos -= cameraSpeed * cameraFront;
//        }
//        if (window.getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
//            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//        }
//        if (window.getKeyState(GLFW_KEY_Q) == GLFW_PRESS) {
//            cameraPos += glm::normalize(-cameraUp) * cameraSpeed;
//        }
//        if (window.getKeyState(GLFW_KEY_E) == GLFW_PRESS) {
//            cameraPos += glm::normalize(cameraUp) * cameraSpeed;
//        }
//        camera.setCameraPos(cameraPos);
//    };
//
//    const float mouseSensitivity = 0.2f;
//
//    auto onMouseMove = [&lastMousePos, &mouseSensitivity, &camera, &window](std::pair<float, float> position) {
//        float xOffset = position.first - lastMousePos.first;
//        float yOffset = lastMousePos.second - position.second;
//
//        xOffset *= mouseSensitivity;
//        yOffset *= mouseSensitivity;
//
//        if (window.getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
//            camera.setYaw(camera.getYaw() + xOffset);
//            camera.setPitch(camera.getPitch() + yOffset);
//        }
//
//        lastMousePos = position;
//    };
//
//    auto onViewSizeChange = [&camera, &graphics](std::pair<int, int> newSize) {
//        camera.setAspectRatio(static_cast<float>(newSize.first) / static_cast<float>(newSize.second));
//        graphics.setViewportSize(newSize);
//    };
//
//    auto onMouseScroll = [&camera](std::pair<float, float> offset) {
//        static const float minFov = 1.0f;
//        static const float maxFov = 35.0f;
//
//        camera.setFov(camera.getFov() - offset.second);
//
//        const double fov = camera.getFov();
//
//        if (fov > maxFov) {
//            camera.setFov(maxFov);
//        } else if (fov < minFov) {
//            camera.setFov(minFov);
//        }
//    };
//
//    // Register callbacks
//    window.setMouseMoveCallback(onMouseMove);
//    window.setViewSizeCallback(onViewSizeChange);
//    window.setMouseScrollCallback(onMouseScroll);
//
//    std::vector<graphics::Vertex> cubeVertices({
//                                                       graphics::Vertex(0.5f, 0.5f, 0.5f),
//                                                       graphics::Vertex(-0.5f, 0.5f, 0.5f),
//                                                       graphics::Vertex(-0.5f, -0.5f, 0.5f),
//                                                       graphics::Vertex(0.5f, -0.5f, 0.5f),
//                                                       graphics::Vertex(0.5f, 0.5f, -0.5f),
//                                                       graphics::Vertex(-0.5f, 0.5f, -0.5f),
//                                                       graphics::Vertex(-0.5f, -0.5f, -0.5f),
//                                                       graphics::Vertex(0.5f, -0.5f, -0.5f),
//                                               });
//
//    std::vector<uint> cubeIndices({
//                                          3, 2, 1,
//                                          3, 1, 0,
//                                          7, 3, 0,
//                                          7, 0, 4,
//                                          6, 7, 4,
//                                          6, 4, 5,
//                                          2, 6, 5,
//                                          2, 5, 1,
//                                          5, 4, 0,
//                                          5, 0, 1,
//                                          2, 3, 7,
//                                          2, 7, 6,
//                                  });
//
//    auto cube = std::make_shared<graphics::Mesh>(cubeVertices, cubeIndices);
//
//    auto destinationMarker = std::make_shared<graphics::Drawable>(cube);
//    destinationMarker->setColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
//    destinationMarker->setScale(glm::vec3(10.0f));
//    auto car = std::make_shared<graphics::Drawable>(cube);
//    car->setScale(glm::vec3(1.9f, 1.5f, 4.7f));
//    car->setColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
//    graphics.draw(car);
//    graphics.draw(destinationMarker);
//
//    // Initialize basic models and objects
//
//    auto tree = map::MapXMLTree("/home/hiram/Projects/citty/samples/sample_map.osm");
//    auto network = tree.generateNetwork();
//    map::TransitNetworkRenderer networkRenderer(network);
//
//    int currentNodeIndex = 0;
//    auto currentWay = network.getHighways().at(10);
//
//    auto initialNode = network.getClosestNodeToCoord(glm::vec3(200.0f, 0.0f, -300.0f));
//    auto targetNode = network.getClosestNodeToCoord(glm::vec3(-1500.0f, 0.0f, 800.0f));
//
//
//    car->setPosition(initialNode->getPosition());
//    destinationMarker->setPosition(targetNode->getPosition());
//
//    camera.setCameraPos(car->getPosition());
//
//    std::cout << network.getHighwayCount() << std::endl;
//    std::cout << network.getNodeCount() << std::endl;
//
//    std::cout << "Initial node: " << initialNode->getId() << std::endl;
//
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
//    networkRenderer.render(graphics);
//    // Render loop
//    while (!window.shouldWindowClose()) {
//        lastFrame = std::chrono::steady_clock::now(); // Store starting time-point of current frame;
//
//        std::cout << "FPS: " << 1000.0f / deltaTime.count() << std::endl;
//        // expansion begin, add anything in here
//
//        // expansion end
//        handleInput();
//        graphics.update();
//        glfwPollEvents();
//        deltaTime = std::chrono::steady_clock::now() - lastFrame;
//        auto extraTime = frameDuration - deltaTime;
//        if (extraTime.count() > 0) {
//            std::this_thread::sleep_for(extraTime);
//        }
//
//        deltaTime = std::chrono::steady_clock::now() - lastFrame;
//    }
//
////    pathfindingThread.join();
//    return 0;
//}