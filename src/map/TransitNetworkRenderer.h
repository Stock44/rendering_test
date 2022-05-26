//
// Created by hiram on 5/19/22.
//

#ifndef CITYY_TRANSITNETWORKRENDERER_H
#define CITYY_TRANSITNETWORKRENDERER_H

#include <span>
#include "../graphics/GraphicsEngine.h"
#include "TransitNetwork.h"
#include "../graphics/Drawable.h"

namespace map {
    using namespace graphics;

    // TODO improve rendering of intersections
    // TODO implement support for updating the map if the transit network changes.
    class TransitNetworkRenderer {
    public:
        explicit TransitNetworkRenderer(TransitNetwork &network);

        void render(GraphicsEngine &graphics);


    private:
        static const inline std::vector<graphics::Vertex> roadVertices = {
                graphics::Vertex(-0.5f, 0.0f, -0.5f),
                graphics::Vertex(-0.5f, 0.0f, 0.5f),
                graphics::Vertex(0.5f, 0.0f, 0.5f),
                graphics::Vertex(0.5f, 0.0f, -0.5f),
        };


        static const inline std::vector<uint> roadIndices = {
                2, 1, 0,
                3, 2, 0,
        };

        static const inline auto road = std::make_shared<graphics::Mesh>(roadVertices, roadIndices);

        // Each highway (per ID) has a vector of 3D graphics objects that represents it
        std::vector<DrawablePtr> objects;
        TransitNetwork &network;
    };

} // map

#endif //CITYY_TRANSITNETWORKRENDERER_H
