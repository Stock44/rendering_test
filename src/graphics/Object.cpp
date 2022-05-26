#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"
//
// Created by hiram on 5/4/22.
//

#include "Object.h"

#include <utility>
#include <glm/ext/matrix_transform.hpp>

namespace graphics {
    Object::Object(std::shared_ptr<Mesh> mesh) : mesh(mesh) {}

    Object::Object(std::shared_ptr<Mesh> mesh, glm::vec3 position) : mesh(mesh), position(position) {
        updateModelMatrix();
        dirty = false;
    }

    Object::Object(std::shared_ptr<Mesh> mesh, float rotationAngle, glm::vec3 rotationAxis) : mesh(mesh),
                                                                                               rotationAngle(
                                                                                                        rotationAngle),
                                                                                               rotationAxis(
                                                                                                        rotationAxis) {
        updateModelMatrix();
        dirty = false;
    }

    Object::Object(std::shared_ptr<Mesh> mesh, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis)
            : mesh(mesh), rotationAngle(rotationAngle), rotationAxis(rotationAxis), position(position) {
        updateModelMatrix();
        dirty = false;
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
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, rotationAxis);
        modelMatrix = glm::scale(modelMatrix, scale);

        dirty = true;
    }

    std::shared_ptr<Mesh> Object::getMesh() {
        return mesh;
    }

    bool Object::isDirty() {
        return dirty;
    }

    void Object::setDirty(bool isDirty) {
        dirty = isDirty;
    }

    const glm::vec3 &Object::getScale() const {
        return scale;
    }

    const glm::vec3 &Object::getPosition() const {
        return position;
    }

    const glm::vec3 &Object::getRotationAxis() const {
        return rotationAxis;
    }

    float Object::getRotationAngle() const {
        return rotationAngle;
    }

    Object::Object(std::shared_ptr<Mesh> model, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis,
                   glm::vec3 scale) : mesh(model), rotationAngle(rotationAngle), rotationAxis(rotationAxis),
                                      position(position), scale(scale) {
        updateModelMatrix();
        dirty = false;
    }

    Object::Object(std::shared_ptr<Mesh> model, glm::vec4 color, glm::vec3 position, float rotationAngle,
                   glm::vec3 rotationAxis, glm::vec3 scale) : mesh(model), color(color), rotationAngle(rotationAngle), rotationAxis(rotationAxis),
                                                              position(position), scale(scale){
        updateModelMatrix();
        dirty = false;
    }

    const glm::vec4 &Object::getColor() const {
        return color;
    }

    void Object::setColor(const glm::vec4 &color) {
        dirty = true;
        Object::color = color;
    }
} // graphics