//
// Created by hiram on 5/30/22.
//

#pragma once

#include <concepts>
#include <map>
#include <tuple>
#include <functional>
#include <memory>
#include <set>
#include <citty/engine/Entity.hpp>

namespace engine {
    class AbstractComponentStore {
    public:
        virtual ~AbstractComponentStore() = default;

        virtual void executeEventCallbacks() = 0;
    };

    using EntitySet = std::set<Entity> const &;
    using ComponentEventCallback = std::function<void(EntitySet)>;


    // Components should be copyable and movable
    template<std::copyable ComponentType>
    class ComponentStore : public AbstractComponentStore {
    public:
        [[nodiscard]] std::map<Entity, ComponentType> const &getComponents() const {
            return components;
        }

        [[nodiscard]] ComponentType const &getComponent(Entity entity) const {
            return components.at(entity);
        }

        [[nodiscard]] bool hasComponent(Entity entity) const {
            return components.contains(entity);
        }

        template<std::common_with<ComponentType> T>
        void setComponent(Entity entity, T &&newComponent) {
            bool created;
            std::tie(std::ignore, created) = components.insert_or_assign(entity, std::forward<T>(newComponent));

            // Only set the component as updated if it's creation has been processed before.
            if (created) createdComponents.emplace(entity);
            else if(!createdComponents.contains(entity)){
                updatedComponents.emplace(entity);
            }
        }

        void deleteComponent(Entity entity) {
            if (!components.contains(entity))
                throw std::invalid_argument("Component doesn't exist for this entity.");
            components.erase(entity);

            createdComponents.erase(entity);
            updatedComponents.erase(entity);
            deletedComponents.emplace(entity);
        }

        void onComponentCreation(ComponentEventCallback const &callback) {
            creationEventCallbacks.emplace_back(callback);
        }

        void onComponentUpdate(ComponentEventCallback const &callback) {
            updateEventCallbacks.emplace_back(callback);
        }

        void onComponentDeletion(ComponentEventCallback const &callback) {
            deletionEventCallbacks.emplace_back(callback);
        }

        void executeEventCallbacks() final {
            if (!createdComponents.empty()) {
                for (const auto &callback: creationEventCallbacks) {
                    callback(createdComponents);
                }
                createdComponents.clear();
            }

            if (!updatedComponents.empty()) {
                for (const auto &callback: updateEventCallbacks) {
                    callback(updatedComponents);
                }
                updatedComponents.clear();
            }

            if (!deletedComponents.empty()) {
                for (const auto &callback: deletionEventCallbacks) {
                    callback(deletedComponents);
                }
                deletedComponents.clear();
            }

        }

    private:
        std::map<Entity, ComponentType> components;

        std::set<Entity> createdComponents;
        std::set<Entity> deletedComponents;
        std::set<Entity> updatedComponents;

        std::vector<ComponentEventCallback> creationEventCallbacks;
        std::vector<ComponentEventCallback> updateEventCallbacks;
        std::vector<ComponentEventCallback> deletionEventCallbacks;
    };

} // engine
