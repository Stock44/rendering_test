//
// Created by hiram on 6/22/22.
//

#include <iostream>
#include "InputSystem.h"

namespace input {
    InputSystem::InputSystem(Window &window) : window(window) {}

    void InputSystem::setup(engine::ComponentManager &componentManager) {
        transformStore = componentManager.getComponentStore<Transform>();
        cameraStore = componentManager.getComponentStore<graphics::Camera>();

        cameraStore->onComponentCreation(
                [this](engine::EntitySet entities) {
                    onCameraCreate(*entities.begin());
                });
    }


    void InputSystem::update(engine::EntityManager &elementManager) {
        auto delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::steady_clock::now() - lastPoll);
        lastPoll = std::chrono::steady_clock::now();
        // Only do camera movement if an offset is detected
        if (xOffset != 0 || yOffset != 0) {
            // Camera's current transform
            auto transform = transformStore->getComponent(controlableEntity);

            // Add and clamp to yaw and pitch
            yaw += xOffset * mouseSensitivity * delta.count() / 1000.0f;
            yaw = yaw > 180.0f ? yaw - 360 : yaw;
            yaw = yaw < -180.0f ? yaw + 360 : yaw;

            pitch -= yOffset * mouseSensitivity * delta.count() / 1000.0f;
            pitch = pitch > 90.0f ? pitch - 180 : pitch;
            pitch = pitch < -90.0f ? pitch + 180 : pitch;

            auto rotateMat = glm::mat4(1.0f);

            rotateMat = glm::rotate(rotateMat, glm::radians(pitch), {0.0f, 1.0f, 0.0f});
            rotateMat = glm::rotate(rotateMat, glm::radians(yaw), {0.0f, 0.0f, 1.0f});
            auto target = glm::vec3(rotateMat * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

            transform.rotationAxis = glm::normalize(glm::cross(target, glm::vec3(1.0f ,0.0f, 0.0f)));
            transform.rotationAngle = glm::degrees(glm::acos(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), target)));

            transformStore->setComponent(controlableEntity, transform);
            xOffset = 0;
            yOffset = 0;
        }
        glfwPollEvents();
    }

    void InputSystem::onCameraCreate(engine::Entity entity) {
        controlableEntity = entity;

        window.setMouseMoveCallback([this](std::pair<float, float> mousePosition) {
            if (!transformStore->hasComponent(controlableEntity)) return;
            if (window.getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                xOffset = mousePosition.first - lastMousePosition.first;
                yOffset = mousePosition.second - lastMousePosition.second;

            }
            lastMousePosition = mousePosition;

        });
    }
} // input