//
// Created by hiram on 6/10/22.
//

#ifndef CITYY_MESHREF_H
#define CITYY_MESHREF_H

#include "Mesh.h"

namespace graphics {
    using MeshRef = std::reference_wrapper<Mesh const>;
}

#endif //CITYY_MESHREF_H
