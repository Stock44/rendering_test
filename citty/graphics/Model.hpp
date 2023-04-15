//
// Created by hiram on 4/11/23.
//

#pragma once


#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/Mesh.hpp>
#include <citty/graphics/Material.hpp>
#include <citty/graphics/Texture.hpp>
#include <citty/graphics/components/Graphics.hpp>
#include <vector>
#include <string_view>

namespace citty::graphics {

    struct Model {
        engine::Transform transform;
        std::vector<Graphics> graphics;
        std::vector<std::unique_ptr<Model>> childNodes;
    };

} // graphics