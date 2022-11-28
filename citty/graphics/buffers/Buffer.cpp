//
// Created by hiram on 5/5/22.
//

#include <citty/graphics/buffers/Buffer.hpp>
#include <citty/graphics/OpenGlError.hpp>

namespace graphics {
    Buffer::Buffer() {
        GLuint newID;
        glGenBuffers(1, &newID);
        ID = newID;
        auto error = glad_glGetError();
        if (error) throw OpenGLError(error);
    }

    std::optional<GLuint> Buffer::getID() const {
        return ID;
    }

    Buffer::~Buffer() {
        if (ID.has_value()) {
            glDeleteBuffers(1, &ID.value());
        }
    }

    Buffer::Buffer(Buffer &&other) noexcept : ID(other.ID.value()) {
        other.ID.reset();
    }

    Buffer &Buffer::operator=(Buffer &&other) noexcept {
        ID = other.ID.value();
        other.ID.reset();
        return *this;
    }
} // graphics