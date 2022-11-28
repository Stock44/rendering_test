//
// Created by hiram on 6/10/22.
//

#ifndef CITYY_MESHREF_HPP
#define CITYY_MESHREF_HPP

#include <citty/graphics/components/Mesh.hpp>

namespace graphics {
    using MeshRef = std::reference_wrapper<Mesh const>;
}

#endif //CITYY_MESHREF_HPP
