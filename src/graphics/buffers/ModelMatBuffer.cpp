//
// Created by hiram on 5/5/22.
//

#include <algorithm>
#include <utility>
#include <stdexcept>
#include "ModelMatBuffer.h"
#include "../OpenGlError.h"

namespace graphics {
    long ModelMatBuffer::getSize() const {
        return std::ssize(modelMats);
    }

    void ModelMatBuffer::enableAttribs() {
        glBindBuffer(GL_ARRAY_BUFFER, getID().value());
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
        auto error = glad_glGetError();
        if (error) throw OpenGLError(error);
    }

    void ModelMatBuffer::addModelMats(std::vector<glm::mat4> const &newMats) {
        modelMats.insert(modelMats.end(), newMats.begin(), newMats.end());
    }

    void ModelMatBuffer::setModelMats(std::vector<glm::mat4> &&newMats) {
        modelMats = std::move(newMats);
    }

    void ModelMatBuffer::setModelMats(const std::vector<glm::mat4> &newMats) {
        modelMats = newMats;
    }

    void ModelMatBuffer::addModelMat(glm::mat4 const &newMat) {
        modelMats.emplace_back(newMat);
    }

    void ModelMatBuffer::modifyModelMats(std::vector<glm::mat4> modMats, long modPosition) {
        if (modMats.empty()) return;
        if (modPosition > std::ssize(modelMats)) throw std::invalid_argument("Position is out of bounds");
        if (std::size(modMats) > std::ssize(modelMats) - modPosition)
            throw std::invalid_argument("Too many vertices to modify");

        auto swapStart = std::next(std::begin(modelMats), modPosition);

        std::ranges::swap_ranges(swapStart, std::next(swapStart, std::ssize(modMats)), std::begin(modMats),
                                 std::end(modMats));
    }

    void ModelMatBuffer::upload() {
        glBindBuffer(GL_ARRAY_BUFFER, getID().value());
        glBufferData(GL_ARRAY_BUFFER, std::ssize(modelMats) * static_cast<long>(sizeof(glm::mat4)), &modelMats[0],
                     GL_STATIC_DRAW);
        auto error = glad_glGetError();
        if (error) throw OpenGLError(error);
    }


    void ModelMatBuffer::deleteModelMat(long index) {
        modelMats.erase(std::next(std::begin(modelMats), index));
    }

    void ModelMatBuffer::setModelMat(glm::mat4 const &modMat, long modPosition) {
        modelMats.at(modPosition) = modMat;
    }
} // graphics