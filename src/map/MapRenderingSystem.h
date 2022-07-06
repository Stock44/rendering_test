//
// Created by hiram on 7/1/22.
//

#ifndef CITYY_MAPRENDERINGSYSTEM_H
#define CITYY_MAPRENDERINGSYSTEM_H

#include "../engine/System.h"
#include "../components/Transform.h"
#include "components/Road.h"
#include "components/Node.h"
#include "../graphics/components/Color.h"
#include "../graphics/components/Mesh.h"
#include "../graphics/components/MeshRef.h"

namespace map {

    class MapRenderingSystem : public engine::System {
    public:
        void setup(engine::ComponentManager &componentManager) override;

        void update(engine::EntityManager &elementManager) override;

        void onRoadCreate(engine::EntitySet entities);


        void onTransformCreate(engine::EntitySet entities);

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
        engine::ComponentStore<Node> *nodeStore;
        engine::ComponentStore<Road> *roadStore;
        engine::ComponentStore<Transform> *transformStore;
        engine::ComponentStore<graphics::MeshRef> *meshStore;
        engine::ComponentStore<graphics::Color> *colorStore;
    };

} // map

#endif //CITYY_MAPRENDERINGSYSTEM_H
