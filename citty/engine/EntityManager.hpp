//
// Created by hiram on 6/6/22.
//

#ifndef CITYY_ENTITYMANAGER_HPP
#define CITYY_ENTITYMANAGER_HPP

#include <citty/engine/Entity.hpp>

namespace engine {

    class EntityManager {
    public:
        EntityManager() = default;

        Entity createEntity();

        EntityManager(EntityManager const& other) = delete;

        EntityManager &operator=(EntityManager const &other) = delete;
    private:
        Entity nextEntity = 0;
    };

} // engine

#endif //CITYY_ENTITYMANAGER_HPP
