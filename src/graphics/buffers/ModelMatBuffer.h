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

        void enableAttribs() override;

        int getSize() override;

        void addModelMats(std::vector<glm::mat4> newMats);

        void addModelMat(glm::mat4 newMat);

        void setModelMats(std::vector<glm::mat4> newMats);

        void replaceModelMat(glm::mat4 newMat, int index);

        void deleteModelMat(int index);

        void upload() override;

    private:

        std::vector<glm::mat4> modelMats;
    };

} // graphics

#endif //CITYY_MODELMATBUFFER_H
