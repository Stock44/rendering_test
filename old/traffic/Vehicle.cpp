//
// Created by hiram on 5/26/22.
//

#include <citty/traffic/Vehicle.h>

namespace traffic {
    const glm::vec3 &Vehicle::getPosition() const {
        return position;
    }

    void Vehicle::setPosition(const glm::vec3 &newPosition) {
        Vehicle::position = newPosition;
    }

    float Vehicle::getAngle() const {
        return angle;
    }

    void Vehicle::setAngle(float angle) {
        Vehicle::angle = angle;
    }

    float Vehicle::getSpeed() const {
        return speed;
    }

    void Vehicle::setSpeed(float speed) {
        Vehicle::speed = speed;
    }
} // traffic