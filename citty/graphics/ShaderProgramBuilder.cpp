//
// Created by hiram on 4/4/23.
//

#include <citty/graphics/ShaderProgramBuilder.hpp>
#include <stdexcept>

namespace citty::graphics {
    void ShaderProgramBuilder::addShader(Shader &&shader) {
        shaders.emplace_back(std::move(shader));
    }

    ShaderProgram ShaderProgramBuilder::build() {
        unsigned int programName = glCreateProgram();
        for (auto &shader: shaders) {
            glAttachShader(programName, shader.getShaderName());
        }
        glLinkProgram(programName);

        int success;
        glGetProgramiv(programName, GL_LINK_STATUS, &success);
        if (!success) {
            throw std::runtime_error("shader program build failed");
        }

        return {programName};
    }
} // graphics