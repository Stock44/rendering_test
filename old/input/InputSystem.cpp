//
// Created by hiram on 6/22/22.
//

#include <iostream>
#include <citty/input/InputSystem.hpp>

namespace input {
//    InputSystem::InputSystem(Window &window) : window(window) {}
//
//    void InputSystem::setup(engine::ComponentManager &componentManager) {
//        transformStore = componentManager.getComponentStore<Transform>();
//        cameraStore = componentManager.getComponentStore<graphics::Camera>();
//
//        cameraStore->onComponentCreation(
//                [this](engine::EntitySet entities) {
//                    onCameraCreate(*entities.begin());
//                });
//    }
//
//
//    void InputSystem::update(engine::EntityManager &entityManager) {
//        auto delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
//                std::chrono::steady_clock::now() - lastPoll);
//        lastPoll = std::chrono::steady_clock::now();
//
//        auto transform = transformStore->getComponent(controlableEntity);
//
//        bool running = window.getKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
//        float adjustedMovementSpeed = running ? 1000.0f : movementSpeed;
//
//        if (window.getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//            window.close();
//        }
//
//        if (window.getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
//            transform.position.x += glm::cos(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//            transform.position.y -= glm::sin(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//        } else if (window.getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
//            transform.position.x -= glm::cos(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//            transform.position.y += glm::sin(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//        }
//
//        if (window.getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
//            transform.position.x += glm::sin(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//            transform.position.y += glm::cos(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//        } else if (window.getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
//            transform.position.x -= glm::sin(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//            transform.position.y -= glm::cos(glm::radians(yaw)) * adjustedMovementSpeed * delta.count() / 1000.0f;
//        }
//
//        if (window.getKeyState(GLFW_KEY_Q) == GLFW_PRESS) {
//            transform.position.z -= adjustedMovementSpeed * delta.count() / 1000.0f;
//        } else if (window.getKeyState(GLFW_KEY_E) == GLFW_PRESS) {
//            transform.position.z += adjustedMovementSpeed * delta.count() / 1000.0f;
//        }
//
//        // Camera rotation input handler
//        // Only do camera rotation if an offset is detected
//        if (xOffset != 0 || yOffset != 0) {
//            // Add and clamp to yaw and pitch
//            yaw += xOffset * mouseSensitivity * delta.count() / 1000.0f;
//            yaw = yaw > 180.0f ? yaw - 360.0f : yaw;
//            yaw = yaw < -180.0f ? yaw + 360.0f : yaw;
//
//            pitch -= yOffset * mouseSensitivity * delta.count() / 1000.0f;
//            pitch = pitch > 90.0f ? 90.0f : pitch;
//            pitch = pitch < -90.0f ? -90.0f : pitch;
//
//            transform.rotation = glm::angleAxis(glm::radians(-yaw), glm::vec3(0.0f, 0.0f, 1.0f)) *
//                                 glm::angleAxis(glm::radians(-pitch), glm::vec3(0.0f, 1.0f, 0.0f));
//
//            xOffset = 0;
//            yOffset = 0;
//        }
//
//        transformStore->setComponent(controlableEntity, transform);
//
//        glfwPollEvents();
//    }
//
//    void InputSystem::onCameraCreate(engine::Entity entity) {
//        controlableEntity = entity;
//
//        // TODO disable callback when no camera present
//        window.setMouseMoveCallback([this](std::pair<float, float> mousePosition) {
//            if (!transformStore->hasComponent(controlableEntity)) return;
//            if (window.getMouseButtonState(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
//                xOffset = mousePosition.first - lastMousePosition.first;
//                yOffset = mousePosition.second - lastMousePosition.second;
//
//            }
//            lastMousePosition = mousePosition;
//
//        });
//    }
} // input