//
// Created by hiram on 3/17/23.
//

#pragma once

#include <citty/engine/Archetype.hpp>
#include <citty/engine/Entity.hpp>
#include <citty/engine/Component.hpp>
#include <citty/engine/ArchetypeRecord.hpp>
#include <citty/engine/ArchetypeGraph.hpp>
#include <boost/hana.hpp>

namespace engine {
    class ComponentStore {
    public:
        /**
         * Add a new component to the given entity, constructing it in place using the provided arguments
         * @tparam T Type of the component to add
         * @tparam Args Types of the arguments to the component's constructor
         * @param entity entity for which to add the component
         * @param args the arguments for the component's constructor
         */
        template<Component T, typename ...Args>
        void add(Entity entity, Args &&...args) {
            auto &archetype = entityArchetypes[entity];
            auto nextArchetype = archetypeGraph.next<T>(archetype);

            auto &archetypeRecord = archetypeRecords[archetype];
            if (archetype.get().empty()) {
                archetypeRecord.add(entity);
            }

            // attempt to get the next archetype record, if it doesn't exist, create it as a derived record from the
            // current archetype
            try {
                auto &nextArchetypeRecord = archetypeRecords.at(nextArchetype);
                archetypeRecord.moveToNextArchetype<T>(entity, nextArchetypeRecord, std::forward<Args>(args)...);
            } catch (std::out_of_range const &) {
                archetypeRecords.try_emplace(nextArchetype, archetypeRecord.constructDerivedRecord<T>());
                auto &nextArchetypeRecord = archetypeRecords.at(nextArchetype);
                archetypeRecord.moveToNextArchetype<T>(entity, nextArchetypeRecord, std::forward<Args>(args)...);
            }
            archetype = nextArchetype;
        }

        /**
         * Attempts to get the specified component type assigned to the given entity
         * @tparam T component type
         * @param entity entity for which to query its component
         * @return a reference to the component. Becomes a dangling reference if entity has a component added or removed.
         */
        template<Component T>
        T &get(Entity entity) {
            auto archetype = entityArchetypes.at(entity);
            auto &archetypeRecord = archetypeRecords.at(archetype);
            return archetypeRecord.get<T>(entity);
        }

        /**
         * Attempts to get the specified component type assigned to the given entity
         * @tparam T component type
         * @param entity entity for which to query its component
         * @return a reference to the component. Becomes a dangling reference if entity has a component added or removed.
         */
        template<Component T>
        T const &get(Entity entity) const {
            auto archetype = entityArchetypes.at(entity);
            auto &archetypeRecord = archetypeRecords.at(archetype);
            return archetypeRecord.get<T>(entity);
        }

        /**
         * Attempts to remove the specified component from the specified entity
         * @tparam T type of component to remove
         * @param entity entity from which to remove the component
         */
        template<Component T>
        void remove(Entity entity) {
            auto &archetype = entityArchetypes[entity];
            auto prevArchetype = archetypeGraph.prev<T>(archetype);

            auto &archetypeRecord = archetypeRecords.at(archetype);
            auto &prevArchetypeRecord = archetypeRecords.at(prevArchetype);
            archetypeRecord.moveToPrevArchetype<T>(entity, prevArchetypeRecord);
            archetype = prevArchetype;
        }

        /**
         * Obtains a map of component type indices to range views of all the existing components.
         * @tparam ComponentTypes the component types to get
         * @return the map
         */
        template<Component ...ComponentTypes>
        auto getAll() {
            using namespace boost;
            ArchetypeFlyweight archetype = {typeid(ComponentTypes)...};
            auto supersets = archetypeGraph.getSupersets(archetype);
            auto componentTypesTuple = hana::make_tuple(hana::type_c<ComponentTypes>...);
            auto componentRangePairs = hana::transform(componentTypesTuple,
                                                       [&supersets, this](auto hanaType) {
                                                           using ComponentType = decltype(hanaType)::type;
                                                           using ContainerType = ComponentContainer::Container<ComponentType>;
                                                           using ContainerReferenceType = std::reference_wrapper<ContainerType>;

                                                           std::vector<ContainerReferenceType> componentContainers;

                                                           for (auto const &superset: supersets) {
                                                               auto &record = archetypeRecords.at(superset);
                                                               componentContainers.emplace_back(
                                                                       record.getAll<ComponentType>());
                                                           }

                                                           auto componentRange = componentContainers | std::views::join;

                                                           return hana::make_pair(
                                                                   std::type_index(typeid(ComponentType)),
                                                                   componentRange);
                                                       });
            return hana::make_map(componentRangePairs);
        }

    private:
        ArchetypeGraph archetypeGraph;
        std::unordered_map<Entity, ArchetypeFlyweight> entityArchetypes;
        std::unordered_map<ArchetypeFlyweight, ArchetypeRecord> archetypeRecords;
    };


} // engine