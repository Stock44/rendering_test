//
// Created by hiram on 3/26/23.
//

#pragma once

#include <queue>
#include <citty/engine/ComponentStore.hpp>

namespace citty::engine {

    class EntityIdStore {
    public:
        EntityId newEntityId();

        void freeEntityId(EntityId entity);

    private:
        EntityId nextId = 0;
        std::queue<EntityId> freedIds;
    };

} // engine