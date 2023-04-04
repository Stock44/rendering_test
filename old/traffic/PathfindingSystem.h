//
// Created by hiram on 7/6/22.
//

#pragma once

#include <citty/engine/System.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/map/components/Road.hpp>
#include <citty/map/components/Node.hpp>
#include <citty/traffic/components/Path.h>
#include <citty/traffic/components/Target.h>
#include <glm/vec3.hpp>
#include <unordered_set>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

BOOST_GEOMETRY_REGISTER_POINT_3D(glm::vec3, float, boost::geometry::cs::cartesian, x, y, z);

namespace traffic {
    struct PathProperties {
        citty::Entity entity;
        float cost;
    };

    class PathfindingSystem : public citty::System {
    public:
        void setup(citty::ComponentManager &componentManager) override;

        void update(citty::EntityManager &elementManager) override;

        void tryRegisterRoad(citty::Entity entity);

        void onRoadCreate(citty::EntitySet entities);

        void onTransformCreate(citty::EntitySet entities);

        void onTargetCreate(citty::EntitySet entities);

        std::optional<Path> findPath(citty::Entity originRoadEntity, citty::Entity destinationRoadEntity);

        float calculateRoadCost(map::Road const &road) const;

    private:
        citty::ComponentStore<Transform> *transformStore;
        citty::ComponentStore<map::Road> *roadStore;
        citty::ComponentStore<map::Node> *nodeStore;
        citty::ComponentStore<PathfindTarget> *targetStore;
        citty::ComponentStore<Path> *pathStore;

        // Box to be used as index in the boost rtree
        // Pair each box with a corresponding road entity

        using Box = boost::geometry::model::box<glm::vec3>;
        using BoxRoadPair = std::pair<Box, citty::Entity>;
        using TransformTree = boost::geometry::index::rtree<BoxRoadPair, boost::geometry::index::quadratic<16>>;

        using RoadGraph = boost::adjacency_list<boost::hash_setS, boost::hash_setS, boost::directedS, citty::Entity, PathProperties>;

        // This set contains all the roads that have been already loaded into the rtree and boost graph
        std::unordered_set<citty::Entity> loadedRoads;

        // Data structures for pathfinding
//        NodeTree nodeTree;
        RoadGraph roadGraph;
    };

} // traffic
