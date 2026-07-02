//
// Created by hiram on 6/4/22.
//

#pragma once

#include <vector>
#include <citty/engine/ComponentStore.hpp>
#include <citty/engine/EntityStore.hpp>
#include <citty/engine/Entity.hpp>

namespace citty::engine
{
    class System
    {
    public:
        virtual ~System() = default;

        void setup(ComponentStore* componentStore, EntityStore* entityStore);

        virtual void init() = 0;

        virtual void update() = 0;

    protected:
        inline Entity newEntity() const
        {
            return entityStore->newEntity();
        }

        inline void deleteEntity(const Entity& entity) const
        {
            entityStore->deleteEntity(entity);
        }

        template <Component ...ComponentTypes>
        inline auto getEntities() const
        {
            return componentStore->getAllEntityIds<ComponentTypes...>() | std::views::transform(
                [this](auto const& entityId) { return Entity{entityId, componentStore}; });
        }

        template <Component ...ComponentTypes>
        inline auto getComponents() const
        {
            return componentStore->getAll<ComponentTypes...>();
        }

    private:
        ComponentStore* componentStore = nullptr;
        EntityStore* entityStore = nullptr;
    };
} // engine
