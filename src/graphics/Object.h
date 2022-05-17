//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_OBJECT_H
#define CITYY_OBJECT_H

#include <glm/vec3.hpp>
#include <memory>
#include "Model.h"

namespace graphics {

    // TODO implement object color
    // TODO implement object textures
    class Object {
    public:
        explicit Object(std::shared_ptr<Model> model);

        Object(std::shared_ptr<Model> model, glm::vec3 position);

        Object(std::shared_ptr<Model> model, float rotationAngle, glm::vec3 rotationAxis);

        Object(std::shared_ptr<Model> model, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis);

        [[nodiscard]] std::shared_ptr<Model> getModel();

        [[nodiscard]] glm::mat4 getModelMatrix();

        void setScale(glm::vec3 newScale);

        void setPosition(glm::vec3 newPosition);

        void setRotation(float angle, glm::vec3 axis);

        bool isDirty();

        void setDirty(bool isDirty);

    private:
        void updateModelMatrix();

        bool dirty = false;

        std::shared_ptr<Model> model;

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glm::vec3 scale = glm::vec3(1.0f);
    public:
        const glm::vec3 &getScale() const;

        const glm::vec3 &getPosition() const;

        const glm::vec3 &getRotationAxis() const;

        float getRotationAngle() const;

    private:

        glm::vec3 position = glm::vec3(0.0f);

        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        float rotationAngle = 0.0f;
    };

} // graphics

#endif //CITYY_OBJECT_H
