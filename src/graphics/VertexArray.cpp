//
// Created by hiram on 5/4/22.
//

#include <iostream>
#include "VertexArray.h"
#include "OpenGlError.h"

namespace graphics {

    void VertexArray::bind() const {
        glBindVertexArray(ID.value());
        auto error = glad_glGetError();
        if (error) throw OpenGLError(error);
    }

    VertexArray::VertexArray() { // NOLINT(cppcoreguidelines-pro-type-member-init)
        GLuint newID;
        glGenVertexArrays(1, &newID);
        ID = newID;
        auto error = glad_glGetError();
        if (error) throw OpenGLError(error);
    }

    VertexArray::~VertexArray() {
        if (ID.has_value()){
            glDeleteVertexArrays(1, &ID.value());
        }
    }

    VertexArray::VertexArray(VertexArray &&other)  noexcept {
        ID = other.ID.value();
        other.ID.reset();
    }

    VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
        ID = other.ID.value();
        other.ID.reset();
        return *this;
    }
} // graphics