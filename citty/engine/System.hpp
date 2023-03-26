//
// Created by hiram on 6/4/22.
//

#pragma once

#include <vector>
#include <citty/engine/ComponentManager.hpp>
#include <citty/engine/EntityManager.hpp>

namespace engine {

    class System {
    public:
        virtual ~System() = default;

//        virtual void setup(ComponentManager &componentManager) = 0;

        virtual void update(EntityManager &elementManager) = 0;
    };

} // engine
