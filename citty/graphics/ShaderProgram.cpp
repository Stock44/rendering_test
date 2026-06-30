//
// Created by hiram on 4/4/23.
//

#include <citty/graphics/ShaderProgram.hpp>
#include <citty/graphics/OpenGlError.hpp>

namespace citty::graphics {
    ShaderProgram::ShaderProgram(unsigned int programName) : programName(programName) {}

    ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept {
        programName = other.programName;
        uniformLocations = std::move(other.uniformLocations);
        other.programName = 0;
    }

    ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept {
        if (programName == other.programName) {
            return *this;
        }

        programName = other.programName;
        uniformLocations = std::move(other.uniformLocations);
        other.programName = 0;

        return *this;
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(programName);
    }

    void ShaderProgram::use() {
        glUseProgram(programName);
    }

    int ShaderProgram::getUniformLocation(std::string_view name) {
        auto [it, inserted] = uniformLocations.try_emplace(std::string(name), -1);
        if (inserted) {
            it->second = glGetUniformLocation(programName, it->first.c_str());
        }
        return it->second;
    }

    void ShaderProgram::setUniform(std::string_view name, int value) {
        glProgramUniform1i(programName, getUniformLocation(name), value);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, float value) {
        glProgramUniform1f(programName, getUniformLocation(name), value);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, Eigen::Matrix4f value, bool transpose) {
        glProgramUniformMatrix4fv(programName, getUniformLocation(name), 1, transpose, &value(0));
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, Eigen::Vector3f value) {
        glProgramUniform3fv(programName, getUniformLocation(name), 1, &value[0]);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, int first, int second) {
        glProgramUniform2i(programName, getUniformLocation(name), first, second);
        checkOpenGlErrors();
    }

    void ShaderProgram::dispatchCompute(unsigned int workgroupsX, unsigned int workgroupsY, unsigned int workgroupsZ) {
        use();
        glDispatchCompute(workgroupsX, workgroupsY, workgroupsZ);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, Eigen::Vector4f value) {
        glProgramUniform4fv(programName, getUniformLocation(name), 1, &value[0]);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, unsigned int first, unsigned int second) {
        glProgramUniform2ui(programName, getUniformLocation(name), first, second);
        checkOpenGlErrors();
    }

    void ShaderProgram::setUniform(std::string_view name, unsigned int value) {
        glProgramUniform1ui(programName, getUniformLocation(name), value);
        checkOpenGlErrors();
    }
} // graphics