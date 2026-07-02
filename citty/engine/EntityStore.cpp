//
// Created by hiram on 3/26/23.
//

#include "EntityStore.hpp"

namespace citty::engine
{
    EntityStore::EntityStore(ComponentStore* componentStore) : componentStore(componentStore)
    {
    }


    Entity EntityStore::newEntity()
    {
        return {newEntityId(), componentStore};
    }

    void EntityStore::deleteEntity(const Entity& entity)
    {
        componentStore->removeAll(entity.getId());
        freeEntityId(entity.getId());
    }

    EntityId EntityStore::newEntityId()
    {
        if (!freedIds.empty())
        {
            const EntityId entity = freedIds.front();
            freedIds.pop();
            return entity;
        }

        const EntityId entity = nextId;
        nextId++;
        return entity;
    }

    void EntityStore::freeEntityId(EntityId entity)
    {
        freedIds.push(entity);
    }
} // engine
