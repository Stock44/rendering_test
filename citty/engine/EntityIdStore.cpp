//
// Created by hiram on 3/26/23.
//

#include "EntityIdStore.hpp"

namespace citty::engine {
    EntityId EntityIdStore::newEntityId() {
        if (!freedIds.empty()) {
            EntityId entity = freedIds.front();
            freedIds.pop();
            return entity;
        }

        EntityId entity = nextId;
        nextId++;
        return entity;
    }

    void EntityIdStore::freeEntityId(EntityId entity) {
        freedIds.push(entity);
    }
} // engine