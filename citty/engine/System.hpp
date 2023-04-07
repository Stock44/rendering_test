//
// Created by hiram on 6/4/22.
//

#pragma once

#include <vector>
#include <citty/engine/ComponentStore.hpp>
#include <citty/engine/EntityIdStore.hpp>
#include <citty/engine/Entity.hpp>

namespace citty::engine {

    class System {
    public:
        virtual ~System() = default;

        void setup(ComponentStore *componentStore, EntityIdStore *entityStore);

        virtual void init() = 0;

        virtual void update() = 0;

    protected:
        Entity newEntity();

        void deleteEntity(Entity entity);

        template<Component ...ComponentTypes>
        inline auto getEntities() const {
            auto allEntities = componentStore->getAllEntities<ComponentTypes...>();
            return std::views::transform(std::move(allEntities), [this](EntityId entity) {
                return Entity(entity, *componentStore);
            });
        }

        template<Component ...ComponentTypes>
        inline auto getComponents() {
            return componentStore->getAll<ComponentTypes...>();
        }

    private:
        ComponentStore *componentStore = nullptr;
        EntityIdStore *entityStore = nullptr;
    };

} // engine
