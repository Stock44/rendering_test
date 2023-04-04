//
// Created by hiram on 3/29/23.
//

#pragma once

#include <Eigen/Dense>

namespace citty::graphics {
    struct Material {
        engine::Entity diffuseMap;
        engine::Entity specularMap;
        float shininess;
    };
}