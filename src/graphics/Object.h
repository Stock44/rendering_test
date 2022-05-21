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
        explicit Object(ModelPtr model);

        Object(ModelPtr model, glm::vec3 position);

        Object(ModelPtr model, float rotationAngle, glm::vec3 rotationAxis);

        Object(ModelPtr model, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis);

        Object(ModelPtr model, glm::vec3 position, float rotationAngle, glm::vec3 rotationAxis,
               glm::vec3 scale);

        Object(ModelPtr model, glm::vec4 color, glm::vec3 position, float rotationAngle,
               glm::vec3 rotationAxis, glm::vec3 scale);

        [[nodiscard]] ModelPtr getModel();

        [[nodiscard]] glm::mat4 getModelMatrix();

        void setScale(glm::vec3 newScale);

        void setPosition(glm::vec3 newPosition);

        void setRotation(float angle, glm::vec3 axis);

        bool isDirty();

        void setDirty(bool isDirty);

        [[nodiscard]] const glm::vec3 &getScale() const;

        [[nodiscard]] const glm::vec3 &getPosition() const;

        [[nodiscard]] const glm::vec3 &getRotationAxis() const;

        float getRotationAngle() const;

        [[nodiscard]] const glm::vec4 &getColor() const;

        void setColor(const glm::vec4 &color);

    private:
        void updateModelMatrix();

        bool dirty = false;

        ModelPtr model;

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glm::vec3 scale = glm::vec3(1.0f);
        glm::vec4 color = glm::vec4(1.0f);

        glm::vec3 position = glm::vec3(0.0f);

        glm::vec3 rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        float rotationAngle = 0.0f;
    };

    typedef std::shared_ptr<Object> ObjectPtr;
} // graphics

#endif //CITYY_OBJECT_H
