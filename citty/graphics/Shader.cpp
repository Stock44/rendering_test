//
// Created by hiram on 4/4/23.
//

#include <fstream>
#include <sstream>
#include <citty/graphics/Shader.hpp>

namespace citty::graphics {
    Shader::Shader(std::string_view pathToSrc, ShaderType type) {
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        shaderFile.open(pathToSrc.data());

        std::stringstream shaderSourceStream;

        shaderSourceStream << shaderFile.rdbuf();

        std::string shaderSource = shaderSourceStream.str();
        const char *rawCode = shaderSource.c_str();

        shaderName = glCreateShader(asGlEnum(type));
        glShaderSource(shaderName, 1, &rawCode, nullptr);
        glCompileShader(shaderName);

        int success;
        glGetShaderiv(shaderName, GL_COMPILE_STATUS, &success);

        if (!success) {
            throw std::runtime_error("failed to compile shader");
        }
    }

    Shader::Shader(Shader &&other) noexcept {
        shaderName = other.shaderName;
        other.shaderName = 0;
    }

    Shader &Shader::operator=(Shader &&other) noexcept {
        if (shaderName == other.shaderName) {
            return *this;
        }

        shaderName = other.shaderName;
        other.shaderName = 0;

        return *this;
    }

    Shader::~Shader() {
        glDeleteShader(shaderName);
    }

    unsigned int Shader::getShaderName() const {
        return shaderName;
    }
} // graphics