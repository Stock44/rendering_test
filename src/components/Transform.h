//
// Created by hiram on 6/6/22.
//

#ifndef CITYY_TRANSFORM_H
#define CITYY_TRANSFORM_H


#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Transform {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotationAxis = {0.0f, 0.0f, 1.0f};
    float rotationAngle = 0.0f;
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    [[nodiscard]] glm::mat4 calculateMatrix() const {
        auto modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, rotationAxis);
        modelMatrix = glm::scale(modelMatrix, scale);
        return modelMatrix;
    }
};


#endif //CITYY_TRANSFORM_H
