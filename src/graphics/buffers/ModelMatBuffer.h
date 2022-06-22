//
// Created by hiram on 5/5/22.
//

#ifndef CITYY_MODELMATBUFFER_H
#define CITYY_MODELMATBUFFER_H

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "Buffer.h"

namespace graphics {

    class ModelMatBuffer : public Buffer {
    public:

        [[nodiscard]] long getSize() const override;

        void enableAttribs() override;

        void addModelMat(glm::mat4 const &newMat);

        void addModelMats(std::vector<glm::mat4> const &newMats);

        void setModelMats(std::vector<glm::mat4> &&newMats);

        void setModelMats(std::vector<glm::mat4> const &newMats);

        void modifyModelMats(std::vector<glm::mat4> modMats, long modPosition);

        void setModelMat(glm::mat4 const &modMat, long modPosition);

        void deleteModelMat(long position);

        void upload() override;

    private:

        std::vector<glm::mat4> modelMats;
    };

} // graphics

#endif //CITYY_MODELMATBUFFER_H
