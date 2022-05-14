//
// Created by hiram on 4/21/22.
//

#ifndef CITYY_SHADER_H
#define CITYY_SHADER_H

#include <glm/glm.hpp>
#include <string>
#include "glad/glad.h"

namespace graphics {
    class Shader {
    public:
        GLint ID;
        std::string vertexPath;
        std::string fragmentPath;

        Shader(const char *vertexPath, const char *fragmentPath);

        void init();

        void use() const;

        void setBool(const std::string &name, bool value) const;

        void setInt(const std::string &name, int value) const;

        void setFloat(const std::string &name, float value) const;

        void setMatrix(const std::string &name, glm::mat4 value) const;

        void setVec3(const std::string &name, glm::vec3 value) const;

        void setVec4(const std::string &name, glm::vec4 value) const;

        void setVec2(const std::string &name, glm::vec2 value) const;
    };
}


#endif //CITYY_SHADER_H
