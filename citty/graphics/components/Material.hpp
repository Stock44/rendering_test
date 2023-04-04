//
// Created by hiram on 3/29/23.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct Material {
        Eigen::Vector3f diffuse;
        Eigen::Vector3f specular;
        engine::Entity diffuseMap;
        engine::Entity specularMap;
        engine::Entity normalMap;
        engine::Entity heightMap;

        float shininess;
    };
}