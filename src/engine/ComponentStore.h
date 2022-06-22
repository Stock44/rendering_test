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
#include "Entity.h"

namespace engine {
    class AbstractComponentStore {
    public:
        virtual ~AbstractComponentStore() = default;

        virtual void executeEventCallbacks() = 0;
    };

    using EntityVectorRef = std::vector<Entity> const &;
    using ComponentEventCallback = std::function<void(EntityVectorRef)>;


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
            std::tie(std::ignore, created) = components.insert_or_assign(entityID, std::forward(newComponent));

            if (created) createdComponents.emplace_back(entityID);
            else updatedComponents.emplace_back(entityID);
        }

        void deleteComponent(Entity element) {
            if (!components.contains(element))
                [[unlikely]] throw std::invalid_argument("Component doesn't exist for this element.");
            components.erase(element);
            deletedComponents.emplace_back(element);
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
            for (const auto &callback: creationEventCallbacks) {
                callback(createdComponents);
            }
            createdComponents.clear();

            for (const auto &callback: updateEventCallbacks) {
                callback(updatedComponents);
            }
            updatedComponents.clear();

            for (const auto &callback: deletionEventCallbacks) {
                callback(deletedComponents);
            }
            deletedComponents.clear();
        }

    private:
        std::map<Entity, ComponentType> components;

        std::vector<Entity> createdComponents;
        std::vector<Entity> deletedComponents;
        std::vector<Entity> updatedComponents;

        std::vector<ComponentEventCallback> creationEventCallbacks;
        std::vector<ComponentEventCallback> updateEventCallbacks;
        std::vector<ComponentEventCallback> deletionEventCallbacks;
    };

} // engine

#endif //CITYY_COMPONENTSTORE_H
