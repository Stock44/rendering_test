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
        Entity(EntityId entity, ComponentStore &componentStore) : entity(entity),
                                                                  componentStore(componentStore) {};

        EntityId entity;

        friend inline std::ostream &operator<<(std::ostream &os, const Entity &entity) {
            os << "entity: " << entity.entity;
            return os;
        }

        template<Component T, typename ...Args>
        inline void addComponent(Args &&...args) {
            componentStore.add<T>(entity, std::forward<Args>(args)...);
        }

        template<Component T>
        inline T &getComponent() {
            return componentStore.get<T>(entity);
        }

        template<Component T>
        inline T const &getComponent() const {
            return componentStore.get<T>(entity);
        }

        template<Component T>
        inline void removeComponent() {
            componentStore.remove<T>(entity);
        }

    private:
        ComponentStore &componentStore;
    };

} // engine