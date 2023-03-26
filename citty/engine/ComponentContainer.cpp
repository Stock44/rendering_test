//
// Created by hiram on 3/22/23.
//

#include <citty/engine/ComponentContainer.hpp>


namespace engine {

    void ComponentContainer::addDefaultComponent() {
        componentContainerBase->addDefaultComponent();
    }

    void ComponentContainer::moveComponent(std::size_t index, ComponentContainer &other) {
        componentContainerBase->moveComponent(index, other.componentContainerBase.get());
    }

    ComponentContainer ComponentContainer::constructEmpty() const {
        return ComponentContainer(componentContainerBase->constructEmpty());
    }

    void ComponentContainer::eraseComponent(std::size_t index) {
        componentContainerBase->eraseComponent(index);
    }

    ComponentContainer::ComponentContainer(
            std::unique_ptr<engine::ComponentContainer::AbstractComponentContainer> &&container)
            : componentContainerBase(std::move(container)) {}
}
