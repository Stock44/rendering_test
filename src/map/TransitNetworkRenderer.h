//
// Created by hiram on 5/19/22.
//

#ifndef CITYY_TRANSITNETWORKRENDERER_H
#define CITYY_TRANSITNETWORKRENDERER_H

#include <span>
#include "../graphics/GraphicsEngine.h"
#include "TransitNetwork.h"
#include "../graphics/Object.h"

namespace map {
    using namespace graphics;

    // TODO improve rendering of intersections
    // TODO implement support for updating the map if the transit network changes.
    class TransitNetworkRenderer {
    public:
        TransitNetworkRenderer(TransitNetwork &network, GraphicsEngine &graphics);

        void render();


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

        static const inline auto road = std::make_shared<graphics::Model>("road", roadVertices, roadIndices);

        // Each highway (per ID) has a vector of 3D graphics objects that represent it
        std::unordered_map<int, std::vector<ObjectID>> highwayObjects;
        TransitNetwork &network;
        GraphicsEngine &graphics;
    };

} // map

#endif //CITYY_TRANSITNETWORKRENDERER_H
