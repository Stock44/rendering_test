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

        void registerSystem(std::shared_ptr<System> system);

        void run();

        void stop();

        EntityManager &getEntityManager();

        ComponentManager &getComponentManager();

        Engine(Engine const &other) = delete;

        Engine &operator=(Engine const &other) = delete;

    private:
        void update();

        bool shouldStop;

        EntityManager entityManager;
        ComponentManager componentManager;

        std::vector<std::shared_ptr<System>> systems;
    };

} // engine
