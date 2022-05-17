//
// Created by hiram on 4/23/22.
//

#ifndef CITYY_CAMERA_H
#define CITYY_CAMERA_H


#include <glm/glm.hpp>
#include "Shader.h"

namespace graphics {
    class Camera {
    public:
        Camera(const glm::vec3 &cameraPos, float fov, float aspectRatio, float pitch, float yaw);

        [[nodiscard]] const glm::vec3 &getCameraPos() const;

        void setCameraPos(const glm::vec3 &cameraPos);

        [[nodiscard]] const glm::vec3 &getCameraFront() const;

        [[nodiscard]] const glm::vec3 &getCameraUp() const;

        [[nodiscard]] float getFov() const;

        void setFov(float fov);

        [[nodiscard]] float getPitch() const;

        void setPitch(float pitch);

        [[nodiscard]] float getYaw() const;

        void setYaw(float yaw);

        void use(Shader &shader);

        [[nodiscard]] float getAspectRatio() const;

        void setAspectRatio(float aspectRatio);

    private:
        glm::vec3 cameraPos;
        glm::vec3 cameraFront = glm::dvec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::dvec3(0.0f, 1.0f, 0.0f);


        float aspectRatio = 1.0f;

        float fov = 120.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
    };
}


#endif //CITYY_CAMERA_H
