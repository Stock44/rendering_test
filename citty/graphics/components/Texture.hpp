//
// Created by hiram on 3/30/23.
//

#pragma once

#include <stb_image.h>

namespace citty::graphics {
    struct Texture {
        unsigned char *textureData;

        int width;
        int height;
        int nrChannels;

        explicit Texture(std::string_view pathToTexture) {
            textureData = stbi_load(pathToTexture.data(), &width, &height, &nrChannels, 0);
        }

        Texture(Texture const &other) = delete;

        Texture &operator=(Texture const &other) = delete;

        Texture(Texture &&other) noexcept: textureData(other.textureData), width(other.width), height(other.height),
                                           nrChannels(other.nrChannels) {
            other.textureData = nullptr;
            other.width = 0;
            other.height = 0;
            other.nrChannels = 0;
        }

        Texture &operator=(Texture &&other) noexcept {
            textureData = other.textureData;
            width = other.width;
            height = other.height;
            nrChannels = other.nrChannels;

            other.textureData = nullptr;
            other.width = 0;
            other.height = 0;
            other.nrChannels = 0;
        }

        ~Texture() {
            stbi_image_free(textureData);
        }
    };
}