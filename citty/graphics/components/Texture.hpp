//
// Created by hiram on 3/30/23.
//

#pragma once

namespace citty::graphics {
    struct Texture {
        unsigned char *textureData;

        int width;
        int height;
        int nrChannels;

        Texture(Texture const &other) = delete;

        Texture &operator=(Texture const &other) = delete;

        Texture(Texture &&other) {

        }

        Texture &operator=(Texture &&other) noexcept {

        }

        ~Texture() {

        }
    };
}