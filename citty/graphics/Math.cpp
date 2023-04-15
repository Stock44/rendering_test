//
// Created by hiram on 4/13/23.
//

#include <citty/graphics/Math.hpp>

namespace citty::graphics {
    Eigen::Projective3f perspectiveProjection(float verticalFoV, float aspectRatio, float zNear, float zFar) {
        if (aspectRatio < 0) throw std::domain_error("aspect ratio cannot be negative");
        if (zFar < zNear) throw std::domain_error("frustrum far cannot be less than frustrum near");
        if (zNear < 0) throw std::domain_error("frustrum near cannot be negative");

        auto projection = Eigen::Projective3f::Identity();
        float tanHalfFoV = std::tan(verticalFoV / 2.0f);

        projection(0, 0) /= aspectRatio * tanHalfFoV;
        projection(1, 1) /= tanHalfFoV;
        projection(2, 2) = -(zFar + zNear) / (zFar - zNear);
        projection(3, 2) = -1.0f;
        projection(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
        return projection;
    }

    Eigen::Affine3f
    lookAt(const Eigen::Vector3f &cameraPos, const Eigen::Vector3f &targetPosition, const Eigen::Vector3f &up) {
        Eigen::Vector3f front = (cameraPos - targetPosition).normalized();
        Eigen::Vector3f right = front.cross(up);

        Eigen::Affine3f lookAt;
        lookAt.matrix() << right.x(), right.y(), right.z(), -cameraPos.x(),
                up.x(), up.y(), up.z(), -cameraPos.y(),
                front.x(), front.y(), front.z(), -cameraPos.z(),
                0.0f, 0.0f, 0.0f, 1.0f;

        return lookAt;
    }
}

