//
// Created by hiram on 6/6/22.
//

#pragma once

#include <Eigen/Geometry>
#include <Eigen/Dense>

namespace citty::engine {
    struct Transform {
        Eigen::Quaterniond rotation;
        Eigen::Vector3d position;
        Eigen::Vector3d scale;

//        [[nodiscard]] glm::mat4 calculateMatrix() const {
//            auto modelMatrix = glm::mat4(1.0f);
//            modelMatrix = glm::translate(modelMatrix, position);
//            modelMatrix = modelMatrix * glm::toMat4(rotation);
//            modelMatrix = glm::scale(modelMatrix, scale);
//            return modelMatrix;
//        }
    };
}


