//
// Created by hiram on 4/4/23.
//

#pragma once

#include <citty/graphics/Shader.hpp>
#include <citty/graphics/Texture.hpp>
#include <Eigen/Dense>
#include <span>

namespace citty::graphics {
    class ShaderProgram {
    public:
        explicit ShaderProgram(unsigned int programName = 0);

        ShaderProgram(ShaderProgram const &other) = delete;

        ShaderProgram &operator=(ShaderProgram const &other) = delete;

        ShaderProgram(ShaderProgram &&other) noexcept;

        ShaderProgram &operator=(ShaderProgram &&other) noexcept;

        void use();

        void setUniform(std::string_view name, int value);

        void setUniform(std::string_view name, unsigned int value);

        void setUniform(std::string_view name, int first, int second);

        void setUniform(std::string_view name, unsigned int first, unsigned int second);

        void setUniform(std::string_view name, float value);

        void setUniform(std::string_view name, Eigen::Matrix4f value, bool transpose = false);

        void setUniform(std::string_view name, Eigen::Vector3f value);

        void setUniform(std::string_view name, Eigen::Vector4f value);

        void dispatchCompute(unsigned int workgroupsX, unsigned int workgroupsY, unsigned int workgroupsZ);

        ~ShaderProgram();

    private:
        unsigned int programName;
    };
} // graphics