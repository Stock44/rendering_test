#include "graphics/GraphicsEngine.h"
#include "graphics/Model.h"
#include "graphics/Object.h"
#include "map/MapXMLTree.h"
#include <cmath>
#include <map>
#include <chrono>
#include <memory>
#include <thread>




int main() {
    // Initialize window, camera and graphics engine.
    Window window({500, 500});
    graphics::Camera camera(glm::dvec3(0.0f, 0.0f, 0.0f), 35.0f,
                            1.0f , 0.0f, 0.0f);
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
        const float cameraSpeed = 100.0f * deltaTime.count() / 1000.0f;
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

    auto onMouseScroll = [&] (std::pair<float, float> offset) {
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

    // Set up basic geometry
    std::vector<graphics::Vertex> triangleVertices({
                                                           graphics::Vertex(0.0f, 0.0f, 0.0f),
                                                           graphics::Vertex(0.0f, 2.0f, 0.0f),
                                                           graphics::Vertex(2.0f, 0.0f, 0.0f),
                                                           graphics::Vertex(0.0f, 0.0f, 2.0f),
                                                   });

    std::vector<uint> triangleIndices({
                                              0, 1, 2,
                                              0, 2, 3,
                                              1, 2, 3,
                                              0, 1, 3,
                                      });

    std::vector<graphics::Vertex> cubeVertices({
                                                       graphics::Vertex(0.0f, 0.0f, 0.0f),
                                                       graphics::Vertex(1.0f, 0.0f, 0.0f),
                                                       graphics::Vertex(1.0f, 1.0f, 0.0f),
                                                       graphics::Vertex(0.0f, 1.0f, 0.0f),
                                                       graphics::Vertex(0.0f, 0.0f, 1.0f),
                                                       graphics::Vertex(1.0f, 0.0f, 1.0f),
                                                       graphics::Vertex(1.0f, 1.0f, 1.0f),
                                                       graphics::Vertex(0.0f, 1.0f, 1.0f),
                                               });

    std::vector<uint> cubeIndices({
                                          0, 1, 2,
                                          0, 2, 3,
                                          1, 6, 2,
                                          1, 5, 6,
                                  });

    std::vector<graphics::Vertex> roadVertices({
        graphics::Vertex(-0.5f, 0.0f, -0.5f),
        graphics::Vertex(-0.5f, 0.0f, 0.5f),
        graphics::Vertex(0.5f, 0.0f, 0.5f),
        graphics::Vertex(0.5f, 0.0f, -0.5f),
    });

    std::vector<uint> roadIndices({
        0, 1, 2,
        0, 2, 3,

    });

    // Initialize basic models and objects
    auto cube = std::make_shared<graphics::Model>("cube", cubeVertices, cubeIndices);
    auto pyramid = std::make_shared<graphics::Model>("triangle", triangleVertices, triangleIndices);
    auto road = std::make_shared<graphics::Model>("road", roadVertices, roadIndices);

    map::MapXMLTree tree = {"/home/hiram/Projects/citty/samples/sample_map.osm"};
    auto network = tree.generateNetwork();

    std::cout << network.getHighwayCount() << std::endl;
    std::cout << network.getNodeCount() << std::endl;

    auto highways = network.getHighways();
    for (auto &highway : highways) {
        auto highwayNodes = highway.second->getNodes();
        for (auto index = 1; index != highwayNodes.size(); ++index){
            auto origin = highwayNodes.at(index - 1)->getCoords();
            auto destination = highwayNodes.at(index)->getCoords();
            auto xDistance = destination.x - origin.x;
            auto zDistance = destination.z - origin.z;
            auto distance = sqrt(pow(xDistance, 2) + pow(zDistance, 2));

            auto newObject = std::make_shared<graphics::Object>(road, glm::vec3(destination.x - xDistance, 0.0f, destination.z - zDistance),
                                                                std::atan2(xDistance, zDistance) + std::numbers::pi / 2,
                                                                glm::vec3(0.0f, 1.0f, 0.0f),
                                                                glm::vec3(distance, 1.0f, 1.0f));
            graphics.addObject(newObject);
        }
    }
    // Render loop
    while (window.shouldWindowClose()) {
        lastFrame = std::chrono::steady_clock::now(); // Store starting time-point of current frame;

        std::cout << "FPS: " << 1000.0f / deltaTime.count() << std::endl;
       // expansion begin, add anything in here



       // expansion end
        handleInput();
        graphics.update();
        glfwPollEvents();
        deltaTime = std::chrono::steady_clock::now() - lastFrame;
        auto extraTime = frameDuration - deltaTime;
        if(extraTime.count() > 0){
            std::this_thread::sleep_for(extraTime);
        }

        deltaTime = std::chrono::steady_clock::now() - lastFrame;
    }

    return 0;
}