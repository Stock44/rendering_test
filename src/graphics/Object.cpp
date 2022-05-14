#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"
//
// Created by hiram on 5/4/22.
//

#include "Object.h"

#include <utility>
#include <glm/ext/matrix_transform.hpp>

namespace graphics {
    Object::Object(std::shared_ptr<Model> model) : model(model) {}

    Object::Object(std::shared_ptr<Model> model, glm::vec3 position) : model(model), position(position) {
        modelMatrix = glm::translate(modelMatrix, position);
    }

    Object::Object(std::shared_ptr<Model> model, float rotationAngle, glm::vec3 rotationAxis) : model(model),
                                                                                                rotationAngle(
                                                                                                        rotationAngle),
                                                                                                rotationAxis(
                                                                                                        rotationAxis) {
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, rotationAxis);
    }

    Object::Object(std::shared_ptr<Model> model, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis)
            : model(model), rotationAngle(rotationAngle), rotationAxis(rotationAxis), position(position) {
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, rotationAxis);
        modelMatrix = glm::translate(modelMatrix, position);
    }

    glm::mat4 Object::getModelMatrix() {
        return modelMatrix;
    }

    void Object::setScale(glm::vec3 newScale) {
        scale = newScale;
        updateModelMatrix();
    }

    void Object::setPosition(glm::vec3 newPosition) {
        position = newPosition;
        updateModelMatrix();
    }

    void Object::setRotation(float angle, glm::vec3 axis) {
        rotationAxis = axis;
        rotationAngle = angle;
        updateModelMatrix();
    }

    void Object::updateModelMatrix() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(modelMatrix, scale);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
        modelMatrix = glm::translate(modelMatrix, position);

        dirty = true;
    }

    std::shared_ptr<Model> Object::getModel() {
        return model;
    }

    bool Object::isDirty() {
        return dirty;
    }
} // graphics