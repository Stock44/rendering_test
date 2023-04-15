//
// Created by hiram on 6/6/22.
//

#include <citty/engine/Engine.hpp>

namespace citty::engine {

//    void Engine::registerSystem(std::shared_ptr<System> system) {
//        system->setup(componentManager);
//        systems.push_back(std::move(system));
//    }
    void Engine::init() {
        for (auto const &system: systems) {
            system->init();
        }
    }

    void Engine::update() {
        for (auto const &system: systems) {
            system->update();
        }
    }

    ComponentStore &Engine::getComponentStore() {
        return componentStore;
    }

    EntityIdStore &Engine::getEntityStore() {
        return entityStore;
    }



//    EntityManager &Engine::getEntityManager() {
//        return entityManager;
//    }

//    ComponentManager &Engine::getComponentManager() {
//        return componentManager;
//    }


} // engine