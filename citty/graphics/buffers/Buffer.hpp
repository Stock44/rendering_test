//
// Created by hiram on 5/5/22.
//

#pragma once

#include <cstddef>
#include <optional>

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
        [[nodiscard]] std::optional<unsigned int> getID() const;

    private:
        std::optional<unsigned int> ID = 0;
    };

} // graphics
