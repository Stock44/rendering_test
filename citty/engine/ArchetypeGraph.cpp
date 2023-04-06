//
// Created by hiram on 3/17/23.
//

#include <citty/engine/ArchetypeGraph.hpp>

namespace citty::engine {

    std::set<ArchetypeFlyweight> ArchetypeGraph::getSupersets(ArchetypeFlyweight archetype) const {
        std::stack<ArchetypeFlyweight> unexploredArchetypes;
        std::set<ArchetypeFlyweight> supersets;

        // the graph will probably not be complete, we can't start looking bottom to top, as there could be isolated
        // branches of supersets. We start at the top instead, with all the top supersets.
        for (auto const &endArchetype: endArchetypes) {
            // if this end archetype is a superset of the archetype, include it in the search
            if (std::ranges::includes(endArchetype.get(), archetype.get())) unexploredArchetypes.push(endArchetype);
        }

        // go top to bottom searching for supersets
        while (!unexploredArchetypes.empty()) {
            auto currentArchetype = unexploredArchetypes.top();
            unexploredArchetypes.pop();

            supersets.emplace(currentArchetype);
            for (auto [componentType, nextArchetype]: inEdges.at(currentArchetype)) {
                // if the removed component in the prev archetype is in the target archetype, the prev archetype
                // is not a subset, skip
                if (archetype.get().contains(componentType)) continue;

                // else, it is a subset, so add it to the explore list
                unexploredArchetypes.push(nextArchetype);
            }
        }

        return supersets;
    }
} // engine