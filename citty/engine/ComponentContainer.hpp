//
// Created by hiram on 3/18/23.
//

#pragma once

#include <set>
#include <typeindex>
#include <vector>
#include <any>
#include <unordered_map>
#include <memory>
#include <citty/engine/Component.hpp>

namespace engine {
    /**
     * ComponentContainer offers a type-erased interface to manipulate containers of Component types. It allows
     * the creation of new containers that are able of storing the same type of container as the current one, without
     * knowing the concrete types. Also has methods to attempt to access the inner container as a given type.
     */
    class ComponentContainer {
    public:
        // Inner container type
        template<Component T>
        using Container = std::vector<T>;

        /**
         * Construct a new type erased component container with a specific container type
         * @tparam T type to store in the container
         */
        template<Component T>
        explicit ComponentContainer(Container<T> &&container) : componentContainerBase(
                std::make_unique<ConcreteComponentContainer<T>>

                        (
                                std::move(container)
                        )) {}

        /**
         * Default-constructs a component at the end of the container
         */
//        void addDefaultComponent();

        /**
         * Moves a component at a given index to the end of the other component
         * @param index
         * @param other
         */
        void moveComponent(std::size_t index, ComponentContainer &other);

        /**
         * Constructs a new empty ComponentContainer that contains the same type of Component.
         * @return ComponentContainer that contains the same type
         */
        [[nodiscard]] ComponentContainer constructEmpty() const;

        /**
         * Erases a component at a given index
         * @param index
         */
        void eraseComponent(std::size_t index);

        /**
         * Attempts to retrieve the base container with its type.
         * @tparam T Type that this ComponentContainer should have
         * @return the typed base container
         */
        template<Component T>
        Container<T> &getBaseContainer() {
            return dynamic_cast<ConcreteComponentContainer<T> *>(componentContainerBase.get())->container;
        }

    private:

        struct AbstractComponentContainer {
        public:
//            virtual void addDefaultComponent() = 0;

            virtual void moveComponent(std::size_t index, AbstractComponentContainer *other) = 0;

            [[nodiscard]] virtual std::unique_ptr<AbstractComponentContainer> constructEmpty() const = 0;

            virtual void eraseComponent(std::size_t index) = 0;

            virtual ~AbstractComponentContainer() = default;

        };

        template<Component T>
        struct ConcreteComponentContainer : public AbstractComponentContainer {
            ConcreteComponentContainer() = default;

            explicit ConcreteComponentContainer(Container<T> &&container) : container(std::move(container)) {}

//            void addDefaultComponent() override {
//                container.emplace_back();
//            }

            void moveComponent(std::size_t index, AbstractComponentContainer *other) override {
                auto *concreteOther = dynamic_cast<ConcreteComponentContainer<T> *>(other);
                concreteOther->container.emplace_back(container.at(index));
                container.erase(container.begin() + index);
            }

            [[nodiscard]] std::unique_ptr<AbstractComponentContainer> constructEmpty() const override {
                return std::make_unique<ConcreteComponentContainer<T>>();
            }

            void eraseComponent(std::size_t index) override {
                container.erase(container.begin() + index);
            }

            Container<T> container;
        };

        /**
         * Constructs a new component container with an unknown type contained in an AbstractComponentContainer.
         * Only used internally to construct empty copies of ComponentContainer
         * @param container The abstract component container to use
         */
        explicit ComponentContainer(std::unique_ptr<AbstractComponentContainer> &&container);

        std::unique_ptr<AbstractComponentContainer> componentContainerBase;
    };

}