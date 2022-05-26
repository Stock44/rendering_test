//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_MESH_H
#define CITYY_MESH_H

#include <vector>
#include <optional>

#include "types.h"

namespace graphics {
    class Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<uint> indices);

        [[nodiscard]] const std::vector<Vertex> &getVertices() const;

        [[nodiscard]] const std::vector<uint> &getIndices() const;


    protected:
        std::vector<Vertex> vertices;
        std::vector<uint> indices;
    };

    typedef std::shared_ptr<Mesh> MeshPtr;
} // graphics

#endif //CITYY_MESH_H
