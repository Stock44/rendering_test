//
// Created by hiram on 5/5/22.
//

#include <algorithm>
#include "ModelMatBuffer.h"

namespace graphics {
    void ModelMatBuffer::enableAttribs() {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid *) nullptr);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid *) (sizeof(float) * 4));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid *) (sizeof(float) * 8));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid *) (sizeof(float) * 12));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }

    int ModelMatBuffer::getSize() {
        return modelMats.size();
    }

    void ModelMatBuffer::addModelMats(std::vector<glm::mat4> newMats) {
        modelMats.insert(modelMats.end(), newMats.begin(), newMats.end());
        dirty = true;
    }

    void ModelMatBuffer::setModelMats(std::vector<glm::mat4> newMats) {
        modelMats = newMats;
        dirty = true;
    }

    void ModelMatBuffer::upload() {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, modelMats.size() * sizeof(glm::mat4), &modelMats[0], GL_STATIC_DRAW);
    }

    void ModelMatBuffer::addModelMat(glm::mat4 newMat) {
        modelMats.push_back(newMat);
        dirty = true;
    }

    void ModelMatBuffer::replaceModelMat(glm::mat4 newMat, int index) {
        modelMats.at(index) = newMat;
    }

    void ModelMatBuffer::deleteModelMat(int index) {
        modelMats.erase(modelMats.begin() + index);
        dirty = true;
    }
} // graphics