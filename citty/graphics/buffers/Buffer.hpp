//
// Created by hiram on 5/5/22.
//

#ifndef CITYY_BUFFER_HPP
#define CITYY_BUFFER_HPP

#include <cstddef>
#include <optional>
#include <citty/glad/glad.h>

namespace graphics {

    class Buffer {
    public:
        Buffer();

        virtual ~Buffer();

        [[nodiscard]] virtual long getSize() const = 0;

        virtual void enableAttribs() = 0;

        virtual void upload() = 0;

        Buffer(Buffer const &other) = delete;
        Buffer(Buffer &&other) noexcept;

        Buffer &operator=(Buffer const &other) = delete;
        Buffer &operator=(Buffer &&other) noexcept;

    protected:
        [[nodiscard]] std::optional<GLuint> getID() const;

    private:
        std::optional<GLuint> ID = 0;
    };

} // graphics

#endif //CITYY_BUFFER_HPP
