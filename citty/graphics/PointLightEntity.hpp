//
// Created by hiram on 5/4/23.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct PointLightEntity {
        Eigen::Vector3f position;
        Eigen::Vector3f color;
        float radius;
    };
}

