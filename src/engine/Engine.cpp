//
// Created by hiram on 6/6/22.
//

#include "Engine.h"

namespace engine {

    void Engine::registerSystem(std::unique_ptr<System> system) {
        system->setup(componentManager);
        systems.push_back(std::move(system));
    }

    void Engine::update() {
        for (auto const &system : systems) {
            system->update(elementManager);
        }
    }

    EntityManager &Engine::getEntityManager() {
        return elementManager;
    }

    ComponentManager &Engine::getComponentManager() {
        return componentManager;
    }
} // engine