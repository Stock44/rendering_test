//
// Created by hiram on 4/4/23.
//

#pragma once

#include <string_view>
#include <epoxy/gl.h>

namespace citty::graphics {
    enum class ShaderType {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        COMPUTE = GL_COMPUTE_SHADER,
    };

    class Shader {
    public:
        Shader(std::string_view pathToSrc, ShaderType type);

        Shader(Shader const &other) = delete;

        Shader &operator=(Shader const &other) = delete;

        Shader(Shader &&other) noexcept;

        Shader &operator=(Shader &&other) noexcept;

        ~Shader();

        [[nodiscard]] unsigned int getShaderName() const;

    private:
        unsigned int shaderName;
    };
} // graphics