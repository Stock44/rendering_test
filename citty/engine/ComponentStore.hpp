//
// Created by hiram on 3/17/23.
//

#pragma once

#include <citty/engine/Archetype.hpp>
#include <citty/engine/EntityId.hpp>
#include <citty/engine/Component.hpp>
#include <citty/engine/ArchetypeRecord.hpp>
#include <citty/engine/ArchetypeGraph.hpp>
#include <boost/hana.hpp>
#include <ranges>

namespace citty::engine
{
    class ComponentStore
    {
    public:
        template <Component T>
        bool has(EntityId entityId)
        {
            const auto& archetype = entityArchetypes[entityId];
            return archetype.get().contains(typeid(T));
        }

        /**
         * Add a new component to the given entity, constructing it in place using the provided arguments
         * @tparam T Type of the component to add
         * @tparam Args Types of the arguments to the component's constructor
         * @param entityId entity for which to add the component
         * @param args the arguments for the component's constructor
         */
        template <Component T, typename... Args>
        void add(EntityId entityId, Args&&... args)
        {
            auto& archetype = entityArchetypes[entityId];
            auto nextArchetype = archetypeGraph.next<T>(archetype);

            auto& archetypeRecord = archetypeRecords[archetype];
            if (archetype.get().empty())
            {
                archetypeRecord.add(entityId);
            }

            // attempt to get the next archetype record, if it doesn't exist, create it as a derived record from the
            // current archetype
            try
            {
                auto& nextArchetypeRecord = archetypeRecords.at(nextArchetype);
                archetypeRecord.moveToNextArchetype<
                    T>(entityId, nextArchetypeRecord, std::forward<Args>(args)...);
            }
            catch (std::out_of_range const&)
            {
                archetypeRecords.try_emplace(nextArchetype, archetypeRecord.constructDerivedRecord<T>());
                auto& nextArchetypeRecord = archetypeRecords.at(nextArchetype);
                archetypeRecord.moveToNextArchetype<
                    T>(entityId, nextArchetypeRecord, std::forward<Args>(args)...);
            }
            archetype = nextArchetype;
        }

        /**
         * Attempts to get the specified component type assigned to the given entity
         * @tparam T component type
         * @param entityId entity for which to query its component
         * @return a reference to the component. Becomes a dangling reference if entity has a component added or removed.
         */
        template <Component T>
        T& get(EntityId entityId)
        {
            auto archetype = entityArchetypes.at(entityId);
            auto& archetypeRecord = archetypeRecords.at(archetype);
            return archetypeRecord.get<T>(entityId);
        }

        /**
         * Attempts to get the specified component type assigned to the given entity
         * @tparam T component type
         * @param entityId entity for which to query its component
         * @return a reference to the component. Becomes a dangling reference if entity has a component added or removed.
         */
        template <Component T>
        T const& get(EntityId entityId) const
        {
            auto archetype = entityArchetypes.at(entityId);
            auto& archetypeRecord = archetypeRecords.at(archetype);
            return archetypeRecord.get<T>(entityId);
        }

        /**
         * Attempts to remove the specified component from the specified entity
         * @tparam T type of component to remove
         * @param entityId entity from which to removeAll the component
         */
        template <Component T>
        void remove(EntityId entityId)
        {
            auto& archetype = entityArchetypes[entityId];
            auto prevArchetype = archetypeGraph.prev<T>(archetype);

            auto& archetypeRecord = archetypeRecords.at(archetype);
            auto& prevArchetypeRecord = archetypeRecords.at(prevArchetype);
            archetypeRecord.moveToPrevArchetype<T>(entityId, prevArchetypeRecord);
            archetype = prevArchetype;
        }

        /**
         * Remove all components from this entity
         * @param entityId
         */
        void removeAll(EntityId entityId);

        /**
         * For a given archetype, return all entities that have this archetype or any superset of it.
         * @tparam ComponentTypes
         * @return
         */
        template <Component ...ComponentTypes>
        auto getAllEntityIds() const
        {
            const ArchetypeFlyweight archetype = makeArchetype<ComponentTypes...>();

            return archetypeGraph.getSupersets(archetype) // For each superset of this archetype
                | std::views::transform([this](auto const& superset) // We get a vector of entity ids
                {
                    return archetypeRecords.at(superset).getAllEntityIds();
                })
                | std::views::join; // We join them into one big range
        }

        /**
         * Obtains a map of component type indices to range views of all the existing components. Component types may
         * not be repeated
         * @tparam ComponentTypes the component types to get
         * @return the map
         */
        template <Component ...ComponentTypes>
        auto getAll()
        {
            using boost::hana::make_tuple;
            using boost::hana::type_c;
            using boost::hana::transform;
            using boost::hana::unpack;

            using std::ranges::ref_view;
            using std::ranges::owning_view;
            using std::views::join;
            using std::ranges::zip_view;

            ArchetypeFlyweight archetype = makeArchetype<ComponentTypes...>();
            auto supersets = archetypeGraph.getSupersets(archetype);
            auto componentTypes = make_tuple(type_c<ComponentTypes>...);


            auto completeContainers = transform(componentTypes,
                                                [&supersets, this](auto hanaType)
                                                {
                                                    using CurrentComponent = decltype(hanaType)::type;
                                                    using Container = ComponentContainer::Container<CurrentComponent>;
                                                    using ContainerRefView = ref_view<Container>;

                                                    std::vector<ContainerRefView> componentContainers;

                                                    for (auto const& superset : supersets)
                                                    {
                                                        auto& container = archetypeRecords.at(
                                                            superset).getAll<CurrentComponent>();
                                                        componentContainers.emplace_back(container);
                                                    }

                                                    return owning_view(std::move(componentContainers)) | join;
                                                });

            return unpack(completeContainers, [](auto&... views)
            {
                return zip_view(owning_view(std::move(views))...);
            });
        }

    private:
        ArchetypeGraph archetypeGraph;
        std::unordered_map<EntityId, ArchetypeFlyweight> entityArchetypes;
        std::unordered_map<ArchetypeFlyweight, ArchetypeRecord> archetypeRecords;
    };
} // engine
