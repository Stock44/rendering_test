//
// Created by hiram on 5/4/23.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct PointLightEntity {
        Eigen::Vector4f position;
        Eigen::Vector4f color;
        float radius;
    };
}

