//
// Created by hiram on 3/17/23.
//

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <citty/engine/Archetype.hpp>
#include <stack>

namespace citty::engine {

    class ArchetypeGraph {
    public:
        /**
         * Gets the archetype that follows from the current archetype when adding a component type
         * @tparam T the added component type
         * @param archetype The base archetype
         * @return
         */
        template<Component T>
        ArchetypeFlyweight next(ArchetypeFlyweight archetype) {
            // if it doesn't exist, it is initialized as an empty vector
            auto &nextArchetypes = outEdges[archetype];
            inEdges[archetype];
            try {
                return nextArchetypes.at(typeid(T));
            } catch (std::out_of_range const &) {
                Archetype nextArchetypeRaw = archetype.get();
                auto [typeIt, typeInserted] = nextArchetypeRaw.emplace(typeid(T));
                if (!typeInserted) throw std::runtime_error("archetype already contains this type");

                ArchetypeFlyweight nextArchetype{nextArchetypeRaw};

                // if the next archetype is empty, then make that one an end archetype.
                // erase does nothing if this archetype wasn't already in the endArchetypes set
                if (outEdges[nextArchetype].empty()) {
                    endArchetypes.erase(archetype);
                    endArchetypes.emplace(nextArchetype);
                }


                auto &nextArchetypePrevs = inEdges[ArchetypeFlyweight(nextArchetype)];
                nextArchetypePrevs.try_emplace(typeid(T), archetype);
                auto [edgeIt, edgeInserted] = nextArchetypes.try_emplace(typeid(T), nextArchetype);
                return edgeIt->second;
            }
        }

        /**
         * Gets the archetype that is obtained when removing a given component type
         * @tparam the removed component type
         * @param archetype
         * @return
         */
        template<Component T>
        ArchetypeFlyweight prev(ArchetypeFlyweight archetype) {
            // if it doesn't exist, it is initialized as an empty vector
            auto &prevArchetypes = inEdges[archetype];
            auto &nextArchetypes = outEdges[archetype];

            if (nextArchetypes.empty()) {
                endArchetypes.emplace(archetype);
            }

            try {
                return prevArchetypes.at(typeid(T));
            } catch (std::out_of_range const &) {
                Archetype prevArchetype = archetype.get();
                std::size_t removedElements = prevArchetype.erase(typeid(T));
                if (removedElements == 0) throw std::runtime_error("archetype does not contain this type");

                auto &prevArchetypeNexts = outEdges[ArchetypeFlyweight(prevArchetype)];
                prevArchetypeNexts.try_emplace(typeid(T), archetype);
                auto [edgeIt, edgeInserted] = prevArchetypes.try_emplace(typeid(T), prevArchetype);
                return edgeIt->second;
            }
        }

        /**
         * Get a set of the given archetype and all archetypes that are supersets of the archetype.
         * TODO Questionable performance, needs profiling
         * @param archetype
         * @return
         */
        std::set<ArchetypeFlyweight> getSupersets(ArchetypeFlyweight archetype) const;

    private:
        using ArchetypeEdges = std::unordered_map<std::type_index, ArchetypeFlyweight>;

        std::set<ArchetypeFlyweight> endArchetypes; // archetypes that don't have any out edges
        std::unordered_map<ArchetypeFlyweight, ArchetypeEdges> outEdges; // the resulting archetypes for adding a given type
        std::unordered_map<ArchetypeFlyweight, ArchetypeEdges> inEdges; // the resulting archetype for removing a given type
    };

} // engine