//
// Created by hiram on 3/19/23.
//

#pragma once

#include <set>
#include <boost/hana.hpp>
#include <typeindex>
#include <vector>
#include <any>
#include <unordered_map>
#include <map>
#include <memory>
#include <ranges>
#include <citty/engine/Archetype.hpp>
#include <citty/engine/Component.hpp>
#include <citty/engine/ComponentContainer.hpp>
#include <citty/engine/EntityId.hpp>

namespace citty::engine {

    class ArchetypeRecord {
    public:
        template<Component T>
        ArchetypeRecord constructDerivedRecord() const {
            if (componentContainers.contains(typeid(T)))
                throw std::runtime_error("record already contains component type");

            ArchetypeRecord derivedRecord;
            // Add all the current component containers to the derived containers
            for (auto &[componentType, componentContainer]: componentContainers) {
                auto [containerIt, inserted] = derivedRecord.componentContainers.try_emplace(componentType,
                                                                                             componentContainer.constructEmpty());
                if (!inserted) throw std::runtime_error("could not copy component containers");
            }
            // Add the extra component container
            auto [newTypeContainerIt, inserted] = derivedRecord.componentContainers.try_emplace(typeid(T),
                                                                                                ComponentContainer(
                                                                                                        ComponentContainer::Container<T>()));
            if (!inserted) throw std::runtime_error("could not add new component type container");
            return derivedRecord;
        }

        /**
         * Attempts to add an entity to this archetype, assuming this is the record for the empty archetype
         * Throws if the archetype is not empty.
         * @param entity
         */
        void add(EntityId entity);

        /**
         * Moves an entity from this archetype to the next archetype, with T as the component being added.
         * Constructs T in place at the end of the other archetype record, using Args args
         * @tparam T Type of the new component in the next archetype
         * @tparam Args Type of the arguments to the new component
         * @param entity Entity to move
         * @param other other archetype record, which is the same as this one with T
         * @param args arguments for the constructor of T
         */
        template<Component T, typename ...Args>
        void moveToNextArchetype(EntityId entity, ArchetypeRecord &other, Args &&...args) {
            // move each of the entity's components to the end of the next archetype's containers
            for (auto &[componentType, componentContainer]: componentContainers) {
                auto &otherContainer = other.componentContainers.at(componentType);
                componentContainer.moveComponent(entityIndices.at(entity), otherContainer);
            }

            // construct in place the new component at the other archetype's corresponding component container
            other.componentContainers.at(typeid(T)).getBaseContainer<T>().emplace_back(std::forward<Args>(args)...);

            shiftIndicesToOther(entity, other);
        }

        /**
         * Move an entity's components from this record to a previous one, destroying the component of type T
         * @tparam T
         * @param entity
         * @param other
         */
        template<Component T>
        void moveToPrevArchetype(EntityId entity, ArchetypeRecord &other) {
            // move each of the entity's components to the end of the previous archetype's containers
            for (auto &[componentType, componentContainer]: componentContainers) {
                if (componentType == typeid(T)) continue;
                auto &otherContainer = other.componentContainers.at(componentType);
                componentContainer.moveComponent(entityIndices.at(entity), otherContainer);
            }

            componentContainers.at(typeid(T)).eraseComponent(entityIndices.at(entity));

            shiftIndicesToOther(entity, other);
        }

        /**
         * Sets the component value directly using the assignment operator for the base component type.
         * @tparam T The type of the component argument. It can be a value reference, but its base type must satisfy Component
         * @param entity Entity for which to set the component
         * @param component new component value
         */
        template<typename T>
        requires Component<std::remove_cvref<T>>
        void set(EntityId entity, T &&component) {
            using ComponentType = std::remove_cvref<T>;

            std::size_t entityIndex = entityIndices.at(entity);

            componentContainers.at(typeid(ComponentType)).getBaseContainer<ComponentType>().at(
                    entityIndex) = std::forward(component);
        };

        /**
         * Get a reference to a specific component for a given entity
         * @tparam T component type to get
         * @param entity entity for which to get its component
         * @return a reference to the entity's component
         */
        template<Component T>
        T &get(EntityId entity) {
            auto entityIndex = entityIndices.at(entity);
            return componentContainers.at(typeid(T)).getBaseContainer<T>()[entityIndex];
        };

        /**
         * Get a reference to a specific component for a given entity
         * @tparam T component type to get
         * @param entity entity for which to get its component
         * @return a const reference to the entity's component
         */
        template<Component T>
        ComponentContainer::Container<T> const &getAll() const {
            return componentContainers.at(typeid(T)).getBaseContainer<T>();
        };


        /**
         * Get a range view of all the components of the specified type in this archetype. Using a range view
         * enables modification of elements, but disables adding and removing elements
         * @tparam T
         * @return
         */
        template<Component T>
        ComponentContainer::Container<T> &getAll() {
            return componentContainers.at(typeid(T)).getBaseContainer<T>();
        }

        /**
         * Gets a range view of all the entity IDs contained in this archetype.
         * @return
         */
        std::vector<EntityId> const &getAllEntityIds() const {
            return entities;
        }

        /**
         * Remove an entity from this record, erasing all of its components
         * @param entity entity to removeAll
         */
        void remove(EntityId entity);

        std::size_t size() const;

    private:
        /**
         * Shifts an entity's index from its current position in this ArchetypeRecord to the end of the other one
         * Does not actually move the components.
         * @param entity
         * @param other
         */
        void shiftIndicesToOther(EntityId entity, ArchetypeRecord &other);

        std::size_t size_v = 0;
        std::unordered_map<EntityId, std::size_t> entityIndices;
        std::vector<EntityId> entities;
        std::unordered_map<std::type_index, ComponentContainer> componentContainers;
    };
} //engine
