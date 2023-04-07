//
// Created by hiram on 3/17/23.
//
#include <typeinfo>
#include <citty/engine/ComponentStore.hpp>
#include "ArchetypeRecord.hpp"

namespace citty::engine {
    void ComponentStore::removeAll(EntityId entity) {
        auto &archetype = entityArchetypes[entity];
        auto &archetypeRecord = archetypeRecords.at(archetype);
        archetypeRecord.remove(entity);
        // entity now has an empty archetype
        archetype = Archetype();
    }

} // engine