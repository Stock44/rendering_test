//
// Created by hiram on 4/4/23.
//

#pragma once

#include <citty/graphics/Shader.hpp>
#include <citty/graphics/components/Texture.hpp>
#include <Eigen/Dense>

namespace citty::graphics {
    class ShaderProgram {
    public:
        ShaderProgram(unsigned int programName);

        ShaderProgram(ShaderProgram const &other) = delete;

        ShaderProgram &operator=(ShaderProgram const &other) = delete;

        ShaderProgram(ShaderProgram &&other) noexcept;

        ShaderProgram &operator=(ShaderProgram &&other) noexcept;

        void setUniform(std::string_view name, int value);

        void setUniform(std::string_view name, Eigen::Matrix4f value);

        void setUniform(std::string_view name, Eigen::Vector3f value);

        ~ShaderProgram();

    private:
        unsigned int programName;
    };
} // graphics