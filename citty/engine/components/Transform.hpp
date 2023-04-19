//
// Created by hiram on 6/6/22.
//

#pragma once

#include <Eigen/Geometry>
#include <optional>
#include <citty/engine/Entity.hpp>

namespace citty::engine {
    struct Transform {
        Eigen::Quaternionf rotation{1.0f, 0.0f, 0.0f, 0.0f};
        Eigen::Vector3f position{0.0f, 0.0f, 0.0f};
        Eigen::Vector3f scale{1.0f, 1.0f, 1.0f};
        std::optional<Entity> parent{};
    };
}


