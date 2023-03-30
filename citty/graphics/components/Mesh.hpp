//
// Created by hiram on 6/6/22.
//
#pragma once

#include <Eigen/Dense>
#include <vector>

namespace citty::graphics {
    struct Mesh {
        std::vector<Eigen::Vector3d> vertices;
        std::vector<unsigned int> indices;
        engine::Entity material;
        bool loaded = false;
    };
}

