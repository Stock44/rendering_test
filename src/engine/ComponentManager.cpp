//
// Created by hiram on 5/30/22.
//

#include "ComponentManager.h"

namespace engine {
    void ComponentManager::handleEvents() const {
        for (const auto &[type, store]: componentStoreMap) {
            store->executeEventCallbacks();
        }
    }
} // engine