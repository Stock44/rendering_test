//
// Created by hiram on 4/4/23.
//

#include <citty/graphics/Image.hpp>

namespace citty {
    namespace graphics {
        Image::Image(Image &&other) noexcept: data(other.data), width(other.width), height(other.height),
                                              nrChannels(other.nrChannels) {
            other.data = nullptr;
            other.width = 0;
            other.height = 0;
            other.nrChannels = 0;
        }

        Image &Image::operator=(Image &&other) noexcept {
            data = other.data;
            width = other.width;
            height = other.height;
            nrChannels = other.nrChannels;

            other.data = nullptr;
            other.width = 0;
            other.height = 0;
            other.nrChannels = 0;
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

        Image::Image(std::string_view pathToTexture) {
            data = stbi_load(pathToTexture.data(), &width, &height, &nrChannels, 0);
        }

        Image::~Image() {
            stbi_image_free(data);
        }
    } // citty
} // graphics