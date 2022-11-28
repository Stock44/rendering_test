
// Created by hiram on 5/30/22.
//

#ifndef CITYY_COMPONENTMANAGER_HPP
#define CITYY_COMPONENTMANAGER_HPP

#include <typeinfo>
#include <concepts>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <citty/engine/ComponentStore.hpp>

namespace engine {

    class ComponentManager {
    public:
        ComponentManager() = default;

        template<std::copyable ComponentType>
        ComponentStore<ComponentType> *getComponentStore() {
            auto type = std::type_index(typeid(ComponentType));

            // If the component type hasn't been registered yet, add it to the map.
            if (!componentStoreMap.contains(type))
                componentStoreMap.try_emplace(type,
                                              std::make_unique<ComponentStore<ComponentType>>());

            return (ComponentStore<ComponentType> *) componentStoreMap.at(type).get();
        }

        void handleEvents() const;

        ComponentManager(ComponentManager const &other) = delete;

        ComponentManager &operator=(ComponentManager const &other) = delete;

    private:
        std::unordered_map<std::type_index, std::unique_ptr<AbstractComponentStore>> componentStoreMap;
    };

} // engine

#endif //CITYY_COMPONENTMANAGER_HPP
