//
// Created by hiram on 6/6/22.
//

#ifndef CITYY_ENGINE_H
#define CITYY_ENGINE_H

#include "System.h"
#include "EntityManager.h"

namespace engine {

    class Engine {
    public:
        void registerSystem(std::unique_ptr<System> system);

        EntityManager &getEntityManager();

        ComponentManager &getComponentManager();

        void update();

    private:
        EntityManager elementManager;
        ComponentManager componentManager;
        std::vector<std::unique_ptr<System>> systems;
    };

} // engine

#endif //CITYY_ENGINE_H
