//
// Created by hiram on 5/4/23.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct GraphicsEntity {
        Eigen::Affine3f transform;
        std::size_t materialId{};
        std::size_t meshId{};

        [[nodiscard]] std::size_t key() const {
            return (materialId << 16) | meshId;
        }
    };
}
