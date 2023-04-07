//
// Created by hiram on 3/26/23.
//

#pragma once

#include <citty/engine/Component.hpp>
#include <citty/engine/ComponentStore.hpp>
#include <citty/engine/EntityId.hpp>
#include <ostream>


namespace citty::engine {

    struct Entity {
        Entity(EntityId id, ComponentStore &componentStore) : id(id),
                                                                  componentStore(componentStore) {};

        auto operator<=>(const Entity &rhs) const {
            return id <=> rhs.id;
        }

        bool operator==(const Entity &rhs) const {
            return id == rhs.id;
        }

        friend inline std::ostream &operator<<(std::ostream &os, const Entity &entity) {
            os << "entity: " << entity.id;
            return os;
        }

        template<Component T, typename ...Args>
        inline void addComponent(Args &&...args) {
            componentStore.add<T>(id, std::forward<Args>(args)...);
        }

        template<Component T>
        inline T &getComponent() {
            return componentStore.get<T>(id);
        }

        template<Component T>
        inline T const &getComponent() const {
            return componentStore.get<T>(id);
        }

        template<Component T>
        inline void removeComponent() {
            componentStore.remove<T>(id);
        }

        EntityId const id;

    private:
        ComponentStore &componentStore;
    };

} // engine

template<>
struct std::hash<citty::engine::Entity> {
    std::size_t operator()(citty::engine::Entity const &s) const noexcept {
        return std::hash<citty::engine::EntityId>{}(s.id);
    }
};