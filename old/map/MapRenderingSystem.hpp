//
// Created by hiram on 7/1/22.
//

#pragma once

#include <citty/engine/System.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/map/components/Road.hpp>
#include <citty/map/components/Node.hpp>
#include <citty/graphics/components/Color.hpp>
#include <citty/graphics/components/Mesh.hpp>
#include <citty/graphics/components/MeshRef.hpp>

namespace map {

    class MapRenderingSystem : public citty::System {
    public:
        void setup(citty::ComponentManager &componentManager) override;

        void update(citty::EntityManager &elementManager) override;

        void onRoadCreate(citty::EntitySet entities);


        void onTransformCreate(citty::EntitySet entities);

    private:
        const graphics::Mesh cubeMesh = {
                1,
                {
                        graphics::Vertex(0.5f, 0.5f, 0.5f),
                        graphics::Vertex(-0.5f, 0.5f, 0.5f),
                        graphics::Vertex(0.5f, 0.5f, -0.5f),
                        graphics::Vertex(-0.5f, 0.5f, -0.5f),
                        graphics::Vertex(0.5f, -0.5f, 0.5f),
                        graphics::Vertex(-0.5f, -0.5f, 0.5f),
                        graphics::Vertex(0.5f, -0.5f, -0.5f),
                        graphics::Vertex(-0.5f, -0.5f, -0.5f),
                },
                {
                        3, 2, 1,
                        3, 1, 0,
                        7, 3, 0,
                        7, 0, 4,
                        6, 7, 4,
                        6, 4, 5,
                        2, 6, 5,
                        2, 5, 1,
                        5, 4, 0,
                        5, 0, 1,
                        2, 3, 7,
                        2, 7, 6,
                },
        };
        citty::ComponentStore<Node> *nodeStore;
        citty::ComponentStore<Road> *roadStore;
        citty::ComponentStore<Transform> *transformStore;
        citty::ComponentStore<graphics::MeshRef> *meshStore;
        citty::ComponentStore<graphics::Color> *colorStore;
    };

} // map
