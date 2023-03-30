//
// Created by hiram on 6/6/22.
//

#pragma once

#include <citty/engine/System.hpp>
#include <citty/engine/ComponentStore.hpp>

namespace citty::engine {

    class Engine {
    public:
        Engine() = default;

        Engine(Engine &&other) = default;

        Engine(Engine const &other) = delete;

        Engine &operator=(Engine const &other) = delete;

        Engine &operator=(Engine &&other) = default;

        template<typename T, typename ...Args>
        T *addSystem(Args &&...args) {
            std::unique_ptr<System> system = systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));

            system->setup(&componentStore, &entityStore);
            system->init();

            return static_cast<T *>(system.get());
        }

        void update();

    private:
        ComponentStore componentStore;
        EntityIdStore entityStore;
        std::vector<std::unique_ptr<System>> systems;
    };

} // engine
