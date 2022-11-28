//
// Created by hiram on 4/21/22.
//

#ifndef CITYY_SHADER_HPP
#define CITYY_SHADER_HPP

#include <citty/glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace graphics {
    class Shader {
    public:
        GLint ID;
        std::string vertexPath;
        std::string fragmentPath;

        Shader(const char *vertexPath, const char *fragmentPath);

        void use();

        void setBool(const std::string &name, bool value);

        void setInt(const std::string &name, int value);

        void setFloat(const std::string &name, float value);

        void setMatrix(const std::string &name, glm::mat4 value);

        void setVec3(const std::string &name, glm::vec3 value);

        void setVec4(const std::string &name, glm::vec4 value);

        void setVec2(const std::string &name, glm::vec2 value);
    };
}


#endif //CITYY_SHADER_HPP
