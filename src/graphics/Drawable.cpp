#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"
//
// Created by hiram on 5/4/22.
//

#include "Drawable.h"

#include <utility>
#include <glm/ext/matrix_transform.hpp>

namespace graphics {
    Drawable::Drawable(std::shared_ptr<Mesh> mesh) : mesh(mesh) {}

    Drawable::Drawable(std::shared_ptr<Mesh> mesh, glm::vec3 position) : mesh(mesh), position(position) {
        updateModelMatrix();
        dirty = false;
    }

    Drawable::Drawable(std::shared_ptr<Mesh> mesh, float rotationAngle, glm::vec3 rotationAxis) : mesh(mesh),
                                                                                                  rotationAngle(
                                                                                                        rotationAngle),
                                                                                                  rotationAxis(
                                                                                                        rotationAxis) {
        updateModelMatrix();
        dirty = false;
    }

    Drawable::Drawable(std::shared_ptr<Mesh> mesh, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis)
            : mesh(mesh), rotationAngle(rotationAngle), rotationAxis(rotationAxis), position(position) {
        updateModelMatrix();
        dirty = false;
    }

    glm::mat4 Drawable::getModelMatrix() {
        return modelMatrix;
    }

    void Drawable::setScale(glm::vec3 newScale) {
        scale = newScale;
        updateModelMatrix();
    }

    void Drawable::setPosition(glm::vec3 newPosition) {
        position = newPosition;
        updateModelMatrix();
    }

    void Drawable::setRotation(float angle, glm::vec3 axis) {
        rotationAxis = axis;
        rotationAngle = angle;
        updateModelMatrix();
    }

    void Drawable::updateModelMatrix() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, rotationAxis);
        modelMatrix = glm::scale(modelMatrix, scale);

        dirty = true;
    }

    std::shared_ptr<Mesh> Drawable::getMesh() {
        return mesh;
    }

    bool Drawable::isDirty() {
        return dirty;
    }

    void Drawable::setDirty(bool isDirty) {
        dirty = isDirty;
    }

    const glm::vec3 &Drawable::getScale() const {
        return scale;
    }

    const glm::vec3 &Drawable::getPosition() const {
        return position;
    }

    const glm::vec3 &Drawable::getRotationAxis() const {
        return rotationAxis;
    }

    float Drawable::getRotationAngle() const {
        return rotationAngle;
    }

    Drawable::Drawable(std::shared_ptr<Mesh> model, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis,
                       glm::vec3 scale) : mesh(model), rotationAngle(rotationAngle), rotationAxis(rotationAxis),
                                      position(position), scale(scale) {
        updateModelMatrix();
        dirty = false;
    }

    Drawable::Drawable(std::shared_ptr<Mesh> model, glm::vec4 color, glm::vec3 position, float rotationAngle,
                       glm::vec3 rotationAxis, glm::vec3 scale) : mesh(model), color(color), rotationAngle(rotationAngle), rotationAxis(rotationAxis),
                                                              position(position), scale(scale){
        updateModelMatrix();
        dirty = false;
    }

    const glm::vec4 &Drawable::getColor() const {
        return color;
    }

    void Drawable::setColor(const glm::vec4 &color) {
        dirty = true;
        Drawable::color = color;
    }
} // graphics