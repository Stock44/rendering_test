//
// Created by hiram on 7/6/22.
//

#ifndef CITYY_PATHFINDINGSYSTEM_H
#define CITYY_PATHFINDINGSYSTEM_H

#include "../engine/System.h"
#include "../components/Transform.h"
#include "../map/components/Road.h"
#include "../map/components/Node.h"
#include "components/Path.h"
#include "components/Target.h"
#include <glm/vec3.hpp>
#include <unordered_set>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>

BOOST_GEOMETRY_REGISTER_POINT_3D(glm::vec3, float, boost::geometry::cs::cartesian, x, y, z);

namespace traffic {

    class PathfindingSystem : public engine::System {
    public:
        void setup(engine::ComponentManager &componentManager) override;

        void update(engine::EntityManager &elementManager) override;

        void tryRegisterRoad(engine::Entity entity);

        void onRoadCreate(engine::EntitySet entities);

        void onTransformCreate(engine::EntitySet entities);

        void onTargetCreate(engine::EntitySet entities);

    private:
        engine::ComponentStore<Transform> *transformStore;
        engine::ComponentStore<map::Road> *roadStore;
        engine::ComponentStore<map::Node> *nodeStore;
        engine::ComponentStore<PathfindTarget> *targetStore;
        engine::ComponentStore<Path> *pathStore;

        using Box = boost::geometry::model::box<glm::vec3>;
        using BoxPair = std::pair<Box, engine::Entity>;
        using RoadTree = boost::geometry::index::rtree<BoxPair, boost::geometry::index::quadratic<16>>;

        RoadTree roadTree;
        std::unordered_set<engine::Entity> loadedRoads;
    };

} // traffic

#endif //CITYY_PATHFINDINGSYSTEM_H
