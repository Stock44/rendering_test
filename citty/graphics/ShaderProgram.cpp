//
// Created by hiram on 4/4/23.
//

#include <citty/graphics/ShaderProgram.hpp>
#include <citty/graphics/OpenGlError.hpp>

namespace citty::graphics {
    ShaderProgram::ShaderProgram(unsigned int programName) : programName(programName) {}

    ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept {
        programName = other.programName;
        other.programName = 0;
    }

    ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept {
        if (programName == other.programName) {
            return *this;
        }

        programName = other.programName;
        other.programName = 0;

        return *this;
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(programName);
    }

    void ShaderProgram::use() {
        glUseProgram(programName);
    }

    void ShaderProgram::setUniform(std::string_view name, int value) {
        use();
        glUniform1i(glGetUniformLocation(programName, name.data()), value);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, float value) {
        use();
        glUniform1f(glGetUniformLocation(programName, name.data()), value);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, Eigen::Matrix4f value, bool transpose) {
        use();
        glUniformMatrix4fv(glGetUniformLocation(programName, name.data()), 1, transpose, &value(0));
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, Eigen::Vector3f value) {
        use();
        glUniform3fv(glGetUniformLocation(programName, name.data()), 1, &value[0]);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, int first, int second) {
        use();
        glUniform2i(glGetUniformLocation(programName, name.data()), first, second);
        checkOpenGlErrors();
    }

    void ShaderProgram::dispatchCompute(unsigned int workgroupsX, unsigned int workgroupsY, unsigned int workgroupsZ) {
        use();
        glDispatchCompute(workgroupsX, workgroupsY, workgroupsZ);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, Eigen::Vector4f value) {
        use();
        glUniform4fv(glGetUniformLocation(programName, name.data()), 1, &value[0]);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, unsigned int first, unsigned int second) {
        use();
        glUniform2ui(glGetUniformLocation(programName, name.data()), first, second);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, unsigned int value) {
        use();
        glUniform1ui(glGetUniformLocation(programName, name.data()), value);
        checkOpenGlErrors();
    }
} // graphics