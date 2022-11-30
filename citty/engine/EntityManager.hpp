//
// Created by hiram on 6/6/22.
//

#pragma once

#include <citty/engine/Entity.hpp>

namespace engine {

    class EntityManager {
    public:
        EntityManager() = default;

        Entity createEntity();

        EntityManager(EntityManager const& other) = delete;

        EntityManager &operator=(EntityManager const &other) = delete;
    private:
        Entity nextEntity = 0;
    };

} // engine
