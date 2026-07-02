//
// Created by hiram on 3/26/23.
//

#pragma once

#include <queue>
#include <citty/engine/ComponentStore.hpp>

#include "Entity.hpp"

namespace citty::engine {

    class EntityStore {
    public:
        EntityStore(ComponentStore *componentStore);

        Entity newEntity();

        void deleteEntity(const Entity& entity);

    private:
        EntityId newEntityId();

        void freeEntityId(EntityId entity);

        EntityId nextId = 0;
        std::queue<EntityId> freedIds;

        ComponentStore *componentStore;
    };

} // engine