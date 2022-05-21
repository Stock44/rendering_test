#include "graphics/GraphicsEngine.h"
#include "graphics/Model.h"
#include "graphics/Object.h"
#include "map/MapXMLTree.h"
#include "map/TransitNetworkRenderer.h"
#include <cmath>
#include <map>
#include <chrono>
#include <memory>
#include <thread>

int main() {
    // Initialize window, camera and graphics engine.
    Window window({500, 500});
    graphics::Camera camera(glm::dvec3(0.0f, 1000.0f, 0.0f), 35.0f,
                            1.0f, 0.0f, 0.0f);
    graphics::GraphicsEngine graphics = graphics::GraphicsEngine(window, camera);

    // Keep track of frame duration
    auto lastFrame = std::chrono::steady_clock::now(); // Start time of last frame
    std::chrono::duration<float, std::milli> deltaTime{}; // Duration of last frame in seconds
    const int FPSLimit = 120;
    const auto frameDuration = std::chrono::duration<float, std::milli>(1000.0f / static_cast<float>(FPSLimit));

    // Keep track of last known mouse position
    auto lastMousePos = std::make_pair(0.0f, 0.0f);

    // Input handlers
    auto handleInput = [&] {
        const float cameraSpeed = 200.0f * deltaTime.count() / 1000.0f;
        auto cameraPos = camera.getCameraPos();
        auto cameraFront = camera.getCameraFront();
        auto cameraUp = camera.getCameraUp();
        if (window.getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            window.close();
        }
        if (window.getKeyState(GLFW_KEY_1) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (window.getKeyState(GLFW_KEY_2) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (window.getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (window.getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (window.getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (window.getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (window.getKeyState(GLFW_KEY_Q) == GLFW_PRESS) {
            cameraPos += glm::normalize(-cameraUp) * cameraSpeed;
        }
        if (window.getKeyState(GLFW_KEY_E) == GLFW_PRESS) {
            cameraPos += glm::normalize(cameraUp) * cameraSpeed;
        }
        camera.setCameraPos(cameraPos);
    };

    const float mouseSensitivity = 0.2f;

    auto onMouseMove = [&](std::pair<float, float> position) {
        float xOffset = position.first - lastMousePos.first;
        float yOffset = lastMousePos.second - position.second;

        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        if (window.getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            camera.setYaw(camera.getYaw() + xOffset);
            camera.setPitch(camera.getPitch() + yOffset);
        }

        lastMousePos = position;
    };

    auto onViewSizeChange = [&](std::pair<int, int> newSize) {
        camera.setAspectRatio(static_cast<float>(newSize.first) / static_cast<float>(newSize.second));
        graphics.setViewportSize(newSize);
    };

    auto onMouseScroll = [&](std::pair<float, float> offset) {
        static const float minFov = 1.0f;
        static const float maxFov = 35.0f;

        camera.setFov(camera.getFov() - offset.second);

        const double fov = camera.getFov();

        if (fov > maxFov) {
            camera.setFov(maxFov);
        } else if (fov < minFov) {
            camera.setFov(minFov);
        }
    };

    // Register callbacks
    window.setMouseMoveCallback(onMouseMove);
    window.setViewSizeCallback(onViewSizeChange);
    window.setMouseScrollCallback(onMouseScroll);

    std::vector<graphics::Vertex> cubeVertices({
                                                       graphics::Vertex(0.5f, 0.5f, 0.5f),
                                                       graphics::Vertex(-0.5f, 0.5f, 0.5f),
                                                       graphics::Vertex(-0.5f, -0.5f, 0.5f),
                                                       graphics::Vertex(0.5f, -0.5f, 0.5f),
                                                       graphics::Vertex(0.5f, 0.5f, -0.5f),
                                                       graphics::Vertex(-0.5f, 0.5f, -0.5f),
                                                       graphics::Vertex(-0.5f, -0.5f, -0.5f),
                                                       graphics::Vertex(0.5f, -0.5f, -0.5f),
                                               });

    std::vector<uint> cubeIndices({
                                          3, 2, 1,
                                          3, 1, 0,
                                          7, 3, 0,
                                          7, 0, 4,
                                          6, 7, 4,
                                          6, 4, 5,
                                          2, 6, 5,
                                          2, 5, 1,
                                          5, 4, 0,
                                          5, 0, 1,
                                          2, 3, 7,
                                          2, 7, 6,
                                  });

    auto cube = std::make_shared<graphics::Model>("cube", cubeVertices, cubeIndices);

    auto destinationMarker = std::make_shared<graphics::Object>(cube);
    destinationMarker->setColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    destinationMarker->setScale(glm::vec3(10.0f));
    auto car = std::make_shared<graphics::Object>(cube);
    car->setScale(glm::vec3(1.9f, 1.5f, 4.7f));
    car->setColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    graphics.addObject(car);
    graphics.addObject(destinationMarker);

    // Initialize basic models and objects

    map::MapXMLTree tree = {"/home/hiram/Projects/citty/samples/sample_map.osm"};
    auto network = tree.generateNetwork();
    map::TransitNetworkRenderer networkRenderer(network, graphics);

    int currentNodeIndex = 0;
    auto currentWay = network.getHighways().at(10);

    auto initialNode = network.getClosestNodeToCoord(glm::vec3(200.0f, 0.0f, -300.0f));
    auto targetNode = network.getClosestNodeToCoord(glm::vec3(-1500.0f, 0.0f, 800.0f));


    car->setPosition(initialNode->getCoords());
    destinationMarker->setPosition(targetNode->getCoords());

    camera.setCameraPos(car->getPosition());

    networkRenderer.render();

    std::cout << network.getHighwayCount() << std::endl;
    std::cout << network.getNodeCount() << std::endl;

    std::cout << "Initial node: " << initialNode->getId() << std::endl;

    // A* algorithm implementation
    auto pathfindingThread = std::thread([&] {
        std::unordered_map<int, float> nodeDistances = {{initialNode->getId(), 0}};
        std::unordered_map<int, float> nodeCosts = {
                {initialNode->getId(), glm::distance(initialNode->getCoords(), targetNode->getCoords())}};
        std::unordered_map<int, map::NodePtr> visitedNodes;
        std::unordered_map<int, map::NodePtr> unvisitedNodes = {{initialNode->getId(), initialNode}};
        std::unordered_map<int, map::NodePtr> visitedFrom;

        std::vector<graphics::ObjectID> nodeMarkers;

        while (!window.shouldWindowClose()) {
            map::NodePtr visitorNode;
            // Find the visited node that has the lowest distance
            for (auto nodeRecord: unvisitedNodes) {
                // Don't do anything if this node is not closer than the currently selected node.
                if (!visitorNode) visitorNode = (*unvisitedNodes.begin()).second;

                float currentCompoundValue =
                        nodeDistances.at(visitorNode->getId()) + nodeCosts.at(visitorNode->getId());

                float recordCompoundValue = nodeDistances.at(nodeRecord.first) + nodeCosts.at(nodeRecord.first);

                if (recordCompoundValue >= currentCompoundValue) continue;
                visitorNode = nodeRecord.second;
            }

            if (visitorNode->getId() == targetNode->getId()) break;

            visitedNodes[visitorNode->getId()] = unvisitedNodes.at(visitorNode->getId());
            unvisitedNodes.erase(visitorNode->getId());
            // For the selected node, find its neighbors, check if they're registered,
            for (auto way: visitorNode->getParentWays()) {
                auto lanes = std::reinterpret_pointer_cast<map::Highway>(way.lock())->getLanes();
                auto wayNodes = way.lock()->getNodes();

                std::vector<map::NodePtr> neighbors;
                for (auto nodeIt = wayNodes.begin(); nodeIt != wayNodes.end(); nodeIt++) {
                    auto id = (*nodeIt)->getId();
                    if (visitorNode->getId() != id) continue;

                    if (nodeIt < wayNodes.end() - 1 && lanes.first > 0) neighbors.push_back(*(nodeIt + 1));

                    if (nodeIt != wayNodes.begin() && lanes.second > 0) neighbors.push_back(*(nodeIt - 1));

                    break;
                }
                for (auto node: neighbors) {
                    // Is the previous node in the way connected to the current one?
                    auto tentativeCost =
                            nodeDistances.at(visitorNode->getId()) +
                            glm::distance(visitorNode->getCoords(), node->getCoords());
                    if (nodeDistances.contains(node->getId())) {
                        auto assignedCost = nodeDistances.at(node->getId());
                        if (assignedCost > tentativeCost) {
                            nodeDistances.at(node->getId()) = tentativeCost;
                            visitedFrom.at(node->getId()) = visitorNode;
                        }
                    } else {
                        nodeDistances[node->getId()] =
                                nodeDistances.at(visitorNode->getId()) +
                                glm::distance(visitorNode->getCoords(), node->getCoords());
                        nodeCosts[node->getId()] = glm::distance(node->getCoords(), targetNode->getCoords());
                        unvisitedNodes[node->getId()] = node;
                        visitedFrom[node->getId()] = visitorNode;
                    }

                    // Just for visualization
                    auto marker = std::make_shared<graphics::Object>(cube);
                    marker->setPosition(node->getCoords());
                    marker->setColor(glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
                    marker->setScale(glm::vec3(5.0f));

                    nodeMarkers.push_back(graphics.addObject(marker));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        for (auto object: nodeMarkers) {
            graphics.deleteObject(object);
        }

        nodeMarkers.clear();

        auto marker = std::make_shared<graphics::Object>(cube);
        marker->setPosition(targetNode->getCoords());
        marker->setColor(glm::vec4(0.0f, 1.0f, 0.5f, 1.0f));
        marker->setScale(glm::vec3(20.0f));

        nodeMarkers.push_back(graphics.addObject(marker));

        auto currentNode = targetNode;
        while (visitedFrom.contains(currentNode->getId())) {
            auto prevNode = visitedFrom.at(currentNode->getId());
            float distance = glm::distance(prevNode->getCoords(), currentNode->getCoords());
            auto object = std::make_shared<graphics::Object>(cube);
            float xDistance = currentNode->getCoords().x - prevNode->getCoords().x;
            float zDistance = currentNode->getCoords().z - prevNode->getCoords().z;
            float lineRotation = std::atan2(xDistance, zDistance) + std::numbers::pi_v<float> / 2.0f;
            auto linePosition = prevNode->getCoords() + ((currentNode->getCoords() - prevNode->getCoords()) / 2.0f) +
                                glm::vec3(0.0f, 1.0f, 0.0f);
            object->setPosition(linePosition);
            object->setColor({1.0f, 0.6f, 0.1f, 1.0f});
            object->setScale({distance, 1.0f, 0.5f});
            object->setRotation(lineRotation, glm::vec3(0.0f, 1.0f, 0.0f));
            graphics.addObject(object);
            currentNode = prevNode;
        }


    });

    // Render loop
    while (!window.shouldWindowClose()) {
        lastFrame = std::chrono::steady_clock::now(); // Store starting time-point of current frame;

        std::cout << "FPS: " << 1000.0f / deltaTime.count() << std::endl;
        // expansion begin, add anything in here



        // expansion end
        handleInput();
        graphics.update();
        glfwPollEvents();
        deltaTime = std::chrono::steady_clock::now() - lastFrame;
        auto extraTime = frameDuration - deltaTime;
        if (extraTime.count() > 0) {
            std::this_thread::sleep_for(extraTime);
        }

        deltaTime = std::chrono::steady_clock::now() - lastFrame;
    }

    pathfindingThread.join();
    return 0;
}