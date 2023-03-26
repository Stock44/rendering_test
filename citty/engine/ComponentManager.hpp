
// Created by hiram on 5/30/22.
//

#pragma once

#include <typeinfo>
#include <concepts>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <citty/engine/ComponentStore.hpp>
#include <any>
#include <citty/engine/Archetype.hpp>
#include <citty/engine/ArchetypeGraph.hpp>

namespace engine {
    class ComponentManager {
    public:
        ComponentManager() = default;

        void handleEvents() const;

        ComponentManager(ComponentManager const &other) = delete;

        ComponentManager &operator=(ComponentManager const &other) = delete;

    private:

        ArchetypeGraph archetypes;

//        std::unordered_map<std::type_index, std::unique_ptr<AbstractComponentStore>> componentStoreMap;
    };

} // engine
