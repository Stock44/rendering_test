//
// Created by hiram on 4/4/23.
//

#pragma once

#include <string_view>
#include <stb_image.h>

namespace citty::graphics {
    class Image {
    public:
        explicit Image(std::string_view pathToTexture);

        Image(Image const &other) = delete;

        Image &operator=(Image const &other) = delete;

        Image(Image &&other) noexcept;

        Image &operator=(Image &&other) noexcept;

        [[nodiscard]] unsigned char const *getData() const;

        int getWidth() const;

        int getHeight() const;

        ~Image();

    private:
        unsigned char *data;

        int width;
        int height;
        int nrChannels;
    };
} // graphics