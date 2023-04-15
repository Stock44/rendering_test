//
// Created by hiram on 4/4/23.
//

#pragma once

#include <string_view>
#include <filesystem>

namespace citty::graphics {
    enum class ColorChannels : int {
        G = 1,
        GA = 2,
        RGB = 3,
        RGBA = 4,
    };

    class Image {
    public:
        explicit Image(std::filesystem::path const &pathToTexture);

        Image(Image const &other) = delete;

        Image &operator=(Image const &other) = delete;

        Image(Image &&other) noexcept;

        Image &operator=(Image &&other) noexcept;

        ~Image();

        [[nodiscard]] unsigned char const *getData() const;

        [[nodiscard]] int getWidth() const;

        [[nodiscard]] int getHeight() const;

        [[nodiscard]] ColorChannels getChannels() const;

    private:
        unsigned char *data;
        int width = 0;
        int height = 0;
        ColorChannels channels;
    };
} // graphics