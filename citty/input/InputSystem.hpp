//
// Created by hiram on 6/22/22.
//

#pragma once

#include <chrono>
#include <citty/engine/System.hpp>
#include <citty/Window.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/components/Camera.hpp>

namespace input {

    class InputSystem : public engine::System {
//    public:
//        explicit InputSystem(Window &window);
//
//        void setup(engine::ComponentManager &componentManager) override;
//
//        void update(engine::EntityManager &entityManager) override;
//
//        void onCameraCreate(engine::Entity camera);
//
//    private:
//        Window &window;
//
//        engine::Entity controlableEntity{};
//
//        float mouseSensitivity = 50.0f;
//        float movementSpeed = 100.0f;
//
//        float xOffset = 0;
//        float yOffset = 0;
//
//        float yaw = 0;
//        float pitch = 0;
//
//        std::pair<float, float> lastMousePosition;
//        std::chrono::time_point<std::chrono::steady_clock> lastPoll;
//        engine::ComponentStore<Transform> *transformStore{};
//        engine::ComponentStore<graphics::Camera> *cameraStore{};
    };

} // input
