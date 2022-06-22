//
// Created by hiram on 6/6/22.
//

#include "EntityManager.h"

namespace engine {
    Entity EntityManager::createEntity() {
        return nextEntity++;
    }
} // engine