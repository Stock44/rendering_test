//
// Created by hiram on 6/6/22.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct Vertex {
        Eigen::Vector3f position;
        Eigen::Vector3f normal;
        Eigen::Vector3f color;
        Eigen::Vector2f texCoords;
    };
}