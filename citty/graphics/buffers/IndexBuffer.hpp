//
// Created by hiram on 5/6/22.
//

#pragma once

#include <vector>
#include <cstdlib>
#include <citty/graphics/buffers/Buffer.hpp>

namespace graphics {

    class IndexBuffer : public Buffer {
    public:
        [[nodiscard]] long getSize() const override;

        void enableAttribs() override;

        void addIndex(unsigned int index);

        void addIndices(std::vector<unsigned int> const &newIndices);

        void setIndices(std::vector<unsigned int> const &newIndices);

        void setIndices(std::vector<unsigned int> &&newIndices);

        void modifyIndices(std::vector<unsigned int> modIndices, long modStart);

        void upload() override;

    private:
        std::vector<unsigned int> indices;
    };

} // graphics
