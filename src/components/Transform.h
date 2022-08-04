//
// Created by hiram on 6/6/22.
//

#ifndef CITYY_TRANSFORM_H
#define CITYY_TRANSFORM_H


#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtx/quaternion.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>

struct Transform {
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 scale;

    [[nodiscard]] glm::mat4 calculateMatrix() const {
        auto modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = modelMatrix * glm::toMat4(rotation);
        modelMatrix = glm::scale(modelMatrix, scale);
        return modelMatrix;
    }
};

#endif //CITYY_TRANSFORM_H
