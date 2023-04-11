//
// Created by hiram on 6/6/22.
//

#pragma once

#include <Eigen/Geometry>
#include <optional>
#include <citty/engine/Entity.hpp>

namespace citty::engine {
    struct Transform {
        Eigen::Quaternionf rotation;
        Eigen::Vector3f position;
        Eigen::Vector3f scale;
        std::optional<Entity> parent;
    };
}


