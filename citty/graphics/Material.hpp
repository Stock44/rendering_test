//
// Created by hiram on 3/29/23.
//

#pragma once

#include <citty/engine/Entity.hpp>
#include <Eigen/Dense>

namespace citty::graphics {
    struct Material {
        Eigen::Vector3f diffuse;
        Eigen::Vector3f specular;
        std::size_t diffuseMap;
        std::size_t specularMap;
        std::size_t normalMap;
        std::size_t heightMap;
        float shininess;
    };
}