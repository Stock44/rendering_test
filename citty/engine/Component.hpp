//
// Created by hiram on 3/17/23.
//

#pragma once

namespace citty::engine {
    template<typename T>
    concept Component = std::movable<T>;
}

