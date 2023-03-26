//
// Created by hiram on 3/17/23.
//

#pragma once

namespace engine {
    template<typename T>
    concept Component = std::copyable<T>;
}

