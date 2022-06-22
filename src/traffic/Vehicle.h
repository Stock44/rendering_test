//
// Created by hiram on 5/26/22.
//

#ifndef CITYY_VEHICLE_H
#define CITYY_VEHICLE_H

#include <glm/vec3.hpp>

namespace traffic {

    class Vehicle {
    public:

        [[nodiscard]] const glm::vec3 &getPosition() const;

        void setPosition(const glm::vec3 &newPosition);

        [[nodiscard]] float getAngle() const;

        void setAngle(float angle);

        [[nodiscard]] float getSpeed() const;

        void setSpeed(float speed);

    private:
        glm::vec3 position = glm::vec3(0.0f);
        float angle = 0.0f;
        float speed = 0.0f;
    };

} // traffic

#endif //CITYY_VEHICLE_H
