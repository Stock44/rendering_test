//
// Created by hiram on 6/6/22.
//

#pragma once

#include <citty/engine/System.hpp>
#include <citty/engine/EntityManager.hpp>

namespace engine {

    class Engine {
    public:
        Engine() = default;

        void registerSystem(std::unique_ptr<System> system);

        EntityManager &getEntityManager();

        ComponentManager &getComponentManager();

        void update();

        Engine(Engine const &other) = delete;

        Engine &operator=(Engine const &other) = delete;

    private:
        EntityManager entityManager;
        ComponentManager componentManager;
        std::vector<std::unique_ptr<System>> systems;
    };

} // engine
