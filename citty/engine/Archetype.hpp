//
// Created by hiram on 1/17/23.
//

#pragma once

#include <set>
#include <typeindex>
#include <vector>
#include <any>
#include <unordered_map>
#include <citty/engine/Component.hpp>
#include <map>
#include <memory>
#include <citty/engine/ComponentContainer.hpp>
#include <boost/flyweight.hpp>
#include <boost/flyweight/no_tracking.hpp>

namespace citty::engine {

    using Archetype = std::set<std::type_index>;

    // a flyweight object just stores a reference to the actual object, its inmutable but otherwise allows all other
    // operations.
    using ArchetypeFlyweight = boost::flyweight<Archetype, boost::flyweights::no_tracking>;

    /**
     * Creates an ArchetypeFlyweight using a list of template components. Used only for testing
     * @tparam ComponentTypes
     * @return
     */
    template<Component ...ComponentTypes>
    ArchetypeFlyweight makeArchetype() {
        return ArchetypeFlyweight({std::type_index(typeid(ComponentTypes))...});
    }

}
