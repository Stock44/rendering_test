//
// Created by hiram on 4/26/23.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct PointLight {
        Eigen::Vector3f color;
        float radius;
    };
}


