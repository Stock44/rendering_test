//
// Created by hiram on 3/26/23.
//

#pragma once

#include <vector>
#include <memory>

namespace citty::engine {
    struct EmptyComponent {
    };

    struct SimpleComponent {
        float floatNumber;
        int number;
        bool boolean;

        auto operator<=>(SimpleComponent const &) const = default;
    };

    struct CompoundComponent {
        SimpleComponent embedded;
        int test;

        auto operator<=>(CompoundComponent const &) const = default;
    };

    template<typename T>
    struct ComplexComponent {
        std::vector<T> container;
        std::shared_ptr<T> copyablePtr;
        T value;

        auto operator<=>(ComplexComponent<T> const &) const = default;
    };
}

