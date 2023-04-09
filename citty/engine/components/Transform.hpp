//
// Created by hiram on 6/6/22.
//

#pragma once

#include <Eigen/Geometry>

namespace citty::engine {
    struct Transform {
        Eigen::Quaternionf rotation;
        Eigen::Vector3f position;
        Eigen::Vector3f scale;

//        [[nodiscard]] glm::mat4 calculateMatrix() const {
//            auto modelMatrix = glm::mat4(1.0f);
//            modelMatrix = glm::translate(modelMatrix, position);
//            modelMatrix = modelMatrix * glm::toMat4(rotation);
//            modelMatrix = glm::scale(modelMatrix, scale);
//            return modelMatrix;
//        }
    };
}


