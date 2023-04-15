//
// Created by hiram on 4/4/23.
//

#include <citty/graphics/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>


namespace citty::graphics {
    Image::Image(Image &&other) noexcept: data(other.data), width(other.width), height(other.height),
                                          channels(other.channels) {
        other.data = nullptr;
        other.width = 0;
        other.height = 0;
    }

    Image &Image::operator=(Image &&other) noexcept {
        // they point to the same data
        if (data == other.data) {
            return *this;
        }

        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;

        other.data = nullptr;
        other.width = 0;
        other.height = 0;

        return *this;
    }

    unsigned char const *Image::getData() const {
        return data;
    }

    int Image::getWidth() const {
        return width;
    }

    int Image::getHeight() const {
        return height;
    }

    Image::Image(std::filesystem::path const &pathToTexture) {
        int numberOfChannels;
        data = stbi_load(pathToTexture.c_str(), &width, &height, &numberOfChannels, 0);
        channels = static_cast<ColorChannels>(numberOfChannels);
    }

    Image::~Image() {
        stbi_image_free(data);
    }

    ColorChannels Image::getChannels() const {
        return channels;
    }
} // graphics