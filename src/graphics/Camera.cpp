//
// Created by hiram on 4/23/22.
//

#include "Camera.h"
#include "glad/glad.h"
#include "Shader.h"
#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <utility>


void graphics::Camera::use(Shader &shader) {
    glm::dvec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);

    glm::dmat4 view = glm::dmat4(1.0);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 proj = glm::perspective(glm::radians(fov), (double) viewSize.first / viewSize.second, 0.1, 100.0);

    shader.setMatrix("view", view);
    shader.setMatrix("projection", proj);
}

graphics::Camera::Camera(const glm::vec3 &cameraPos, double fov, std::pair<int, int> viewSize, double pitch,
                         double yaw) : cameraPos(cameraPos), fov(fov), viewSize(std::move(viewSize)), pitch(pitch),
                                       yaw(yaw) {}

const std::pair<int, int> &graphics::Camera::getViewSize() const {
    return viewSize;
}

void graphics::Camera::setViewSize(const std::pair<double, double> &viewSize) {
    Camera::viewSize = viewSize;
}

const glm::vec3 &graphics::Camera::getCameraPos() const {
    return cameraPos;
}

void graphics::Camera::setCameraPos(const glm::vec3 &cameraPos) {
    Camera::cameraPos = cameraPos;
}

const glm::vec3 &graphics::Camera::getCameraFront() const {
    return cameraFront;
}

const glm::vec3 &graphics::Camera::getCameraUp() const {
    return cameraUp;
}

double graphics::Camera::getFov() const {
    return fov;
}

void graphics::Camera::setFov(double fov) {
    Camera::fov = fov;
}

float graphics::Camera::getPitch() const {
    return pitch;
}

void graphics::Camera::setPitch(float pitch) {
    Camera::pitch = pitch;
}

float graphics::Camera::getYaw() const {
    return yaw;
}

void graphics::Camera::setYaw(float yaw) {
    Camera::yaw = yaw;
}
