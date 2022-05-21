//
// Created by hiram on 5/4/22.
//

#ifndef CITYY_MODEL_H
#define CITYY_MODEL_H

#include <vector>
#include <optional>

#include "types.h"

namespace graphics {
    class Model {
    public:
        Model(std::string name, std::vector<Vertex> vertices);

        Model(std::string name, std::vector<Vertex> vertices, std::vector<uint> indices);

        [[nodiscard]] const std::vector<Vertex> &getVertices() const;

        [[nodiscard]] bool usesElements() const;

        [[nodiscard]] const std::vector<uint> &getIndices() const;

        [[nodiscard]] const std::string &getName() const;

    protected:
        std::vector<Vertex> vertices;
        std::optional<std::vector<uint>> indices;
        std::string name;
    };

    typedef std::shared_ptr<Model> ModelPtr;
} // graphics

#endif //CITYY_MODEL_H
