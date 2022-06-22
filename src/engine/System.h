//
// Created by hiram on 6/4/22.
//

#ifndef CITYY_SYSTEM_H
#define CITYY_SYSTEM_H

#include <vector>
#include "ComponentManager.h"
#include "EntityManager.h"

namespace engine {

    class System {
    public:
        virtual ~System() = default;

        virtual void setup(ComponentManager &componentManager) = 0;

        virtual void update(EntityManager &elementManager) = 0;
    };

} // engine

#endif //CITYY_SYSTEM_H
