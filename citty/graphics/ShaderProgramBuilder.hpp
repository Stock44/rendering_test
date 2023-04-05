//
// Created by hiram on 4/4/23.
//

#pragma once

#include <string_view>
#include <vector>
#include <citty/graphics/ShaderProgram.hpp>

namespace citty::graphics {

    class ShaderProgramBuilder {
    public:
        void addShader(Shader &&shader);

        ShaderProgram build();

    private:
        std::vector<Shader> shaders;
    };

} // graphics