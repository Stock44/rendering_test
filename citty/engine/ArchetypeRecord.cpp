//
// Created by hiram on 3/23/23.
//
#include <citty/engine/ArchetypeRecord.hpp>


namespace engine {

    std::size_t engine::ArchetypeRecord::size() const {
        return size_v;
    }

    void ArchetypeRecord::shiftIndicesToOther(Entity entity, ArchetypeRecord &other) {
        auto entityIndex = entityIndices.at(entity);
        // erase entity from this archetype's entities
        entityIndices.erase(entity);
        auto entityIt = entities.begin();
        std::advance(entityIt, entityIndex);
        entities.erase(entityIt);

        // register entity in other archetype
        other.entityIndices.try_emplace(entity, other.size_v);
        other.entities.push_back(entity);

        // modify sizes
        size_v--;
        other.size_v++;

        // Entity indices after this one should be shifted back one value
        for (auto shiftedEntity: entities | std::views::reverse |
                                 std::views::take(size_v - entityIndex)) {
            entityIndices.at(shiftedEntity)--;
        }
    }

    void ArchetypeRecord::add(Entity entity) {
        entityIndices.emplace(entity, entities.size());
        entities.push_back(entity);
        size_v++;
    }

//    void ArchetypeRecord::add(Entity entity) {
//        for (auto &[_, componentContainer]: componentContainers) {
//            componentContainer.addDefaultComponent();
//        }
//
//        entities.emplace_back(entity);
//        entityIndices.try_emplace(entity, size_v);
//        size_v++;
//    }
}
