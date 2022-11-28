//
// Created by hiram on 6/6/22.
//

#include <citty/engine/EntityManager.hpp>

namespace engine {
    Entity EntityManager::createEntity() {
        return nextEntity++;
    }
} // engine