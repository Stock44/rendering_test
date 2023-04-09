//
// Created by hiram on 3/26/23.
//

#include <citty/engine/System.hpp>

namespace citty::engine {
    void System::setup(ComponentStore *componentStore, EntityIdStore *entityStore) {
        this->componentStore = componentStore;
        this->entityStore = entityStore;
    }

    inline Entity System::newEntity() {
        return {entityStore->newEntityId(), *componentStore};
    }

    inline void System::deleteEntity(Entity entity) {
        componentStore->removeAll(entity);
        entityStore->freeEntityId(entity);
    }
}