//
// Created by hiram on 5/30/22.
//

#ifndef CITYY_COMPONENTSTORE_H
#define CITYY_COMPONENTSTORE_H

#include <concepts>
#include <map>
#include <tuple>
#include <functional>
#include <memory>
#include <set>
#include "Entity.h"

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

        [[nodiscard]] ComponentType const &getComponent(Entity entityID) const {
            return components.at(entityID);
        }

        [[nodiscard]] bool hasComponent(Entity entityID) const {
            return components.contains(entityID);
        }

        template<std::common_with<ComponentType> T>
        void setComponent(Entity entityID, T &&newComponent) {
            bool created;
            std::tie(std::ignore, created) = components.insert_or_assign(entityID, std::forward<T>(newComponent));

            if (created) createdComponents.emplace(entityID);
            else updatedComponents.emplace(entityID);
        }

        void deleteComponent(Entity element) {
            if (!components.contains(element))
                [[unlikely]] throw std::invalid_argument("Component doesn't exist for this element.");
            components.erase(element);
            deletedComponents.emplace(element);
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

#endif //CITYY_COMPONENTSTORE_H
