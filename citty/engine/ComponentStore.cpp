//
// Created by hiram on 3/17/23.
//
#include <typeinfo>
#include <citty/engine/ComponentStore.hpp>
#include "ArchetypeRecord.hpp"

namespace citty::engine {
    void ComponentStore::removeAll(EntityId entityId) {
        auto &archetype = entityArchetypes[entityId];
        auto &archetypeRecord = archetypeRecords.at(archetype);
        archetypeRecord.remove(entityId);
        // entity now has an empty archetype
        archetype = Archetype();
    }

} // engine