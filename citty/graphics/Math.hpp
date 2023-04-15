#pragma once

#include <Eigen/Geometry>

namespace citty::graphics {
    Eigen::Projective3f perspectiveProjection(float verticalFoV, float aspectRatio, float zNear, float zFar);

    Eigen::Affine3f
    lookAt(Eigen::Vector3f const &cameraPos, Eigen::Vector3f const &targetPosition, Eigen::Vector3f const &up);

}


