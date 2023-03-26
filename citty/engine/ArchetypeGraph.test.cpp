//
// Created by hiram on 3/26/23.
//

#include <catch2/catch_test_macros.hpp>
#include <citty/engine/ArchetypeGraph.hpp>
#include <citty/engine/components/Transform.hpp>

struct EmptyComponent {
};

struct SimpleComponent {
    float floatNumber;
    int number;
    bool boolean;
};

struct CompoundComponent {
    SimpleComponent embedded;
    int test;
};

template<typename T>
struct ComplexComponent {
    std::vector<T> container;
    std::shared_ptr<T> copyablePtr;
    T value;
};

TEST_CASE ("archetype graph can be traversed both forwards and backwards", "[ArchetypeGraph]") {
    using namespace engine;
    ArchetypeGraph archetypeGraph;

    SECTION("forward traversal through graph") {
        REQUIRE(archetypeGraph.next<EmptyComponent>(makeArchetype<>()).get() == makeArchetype<EmptyComponent>().get());
        REQUIRE(archetypeGraph.next<SimpleComponent>(makeArchetype<EmptyComponent>()).get() ==
                makeArchetype<EmptyComponent, SimpleComponent>().get());
        REQUIRE(archetypeGraph.next<CompoundComponent>(makeArchetype<EmptyComponent, SimpleComponent>()).get() ==
                makeArchetype<EmptyComponent, SimpleComponent, CompoundComponent>().get());
        REQUIRE(archetypeGraph.next<ComplexComponent<int>>(makeArchetype<>()).get() ==
                makeArchetype<ComplexComponent<int>>().get());
        REQUIRE_FALSE(archetypeGraph.next<ComplexComponent<float>>(makeArchetype<>()).get() ==
                      makeArchetype<ComplexComponent<double>>().get());
    }

    SECTION("backward traversal through graph") {
        REQUIRE(archetypeGraph.prev<EmptyComponent>(makeArchetype<EmptyComponent, ComplexComponent<float>>()).get() ==
                makeArchetype<ComplexComponent<float>>().get());
        REQUIRE(archetypeGraph.prev<EmptyComponent>(makeArchetype<EmptyComponent>()).get() ==
                makeArchetype<>().get());
        REQUIRE(archetypeGraph.prev<SimpleComponent>(
                makeArchetype<EmptyComponent, SimpleComponent, ComplexComponent<float>>()).get() ==
                makeArchetype<EmptyComponent, ComplexComponent<float>>().get());
    }

    SECTION("forwards and backwards traversal") {
        REQUIRE(archetypeGraph.next<Transform>(makeArchetype<>()).get() == makeArchetype<Transform>().get());
        REQUIRE(archetypeGraph.next<SimpleComponent>(makeArchetype<Transform>()).get() ==
                makeArchetype<Transform, SimpleComponent>().get());
        REQUIRE(archetypeGraph.next<EmptyComponent>(makeArchetype<Transform, SimpleComponent>()).get() ==
                makeArchetype<Transform, SimpleComponent, EmptyComponent>().get());
        REQUIRE(archetypeGraph.prev<Transform>(makeArchetype<Transform, SimpleComponent, CompoundComponent>()).get() ==
                makeArchetype<SimpleComponent, CompoundComponent>().get());
        REQUIRE(archetypeGraph.prev<SimpleComponent>(makeArchetype<CompoundComponent, SimpleComponent>()).get() ==
                makeArchetype<CompoundComponent>().get());
        REQUIRE(archetypeGraph.prev<CompoundComponent>(
                makeArchetype<CompoundComponent>()).get() == makeArchetype<>().get());
    }


}

TEST_CASE("archetype graph throws with invalid operations", "[ArchetypeGraph]") {
    using namespace engine;
    ArchetypeGraph archetypeGraph;

    SECTION("attempt to add components already in archetype") {
        REQUIRE_THROWS(archetypeGraph.next<Transform>(makeArchetype<Transform>()));
        REQUIRE_THROWS(archetypeGraph.next<SimpleComponent>(makeArchetype<Transform, SimpleComponent>()));
        REQUIRE_THROWS(
                archetypeGraph.next<CompoundComponent>(makeArchetype<Transform, SimpleComponent, CompoundComponent>()));
    }

    SECTION("attempt to remove components not in archetype") {
        REQUIRE_THROWS(archetypeGraph.prev<Transform>(makeArchetype<>()));
        REQUIRE_THROWS(archetypeGraph.prev<Transform>(makeArchetype<SimpleComponent, CompoundComponent>()));
        REQUIRE_THROWS(
                archetypeGraph.prev<ComplexComponent<float>>(makeArchetype<SimpleComponent, ComplexComponent<int>>()));
    }
}

TEST_CASE("archetype graph can be queried for registered supersets", "[ArchetypeGraph]") {
    using namespace engine;
    ArchetypeGraph archetypeGraph;

    SECTION("get all the subsets for a given archetype") {
        auto archetype = archetypeGraph.next<EmptyComponent>(makeArchetype<>());
        archetype = archetypeGraph.next<SimpleComponent>(archetype);
        archetype = archetypeGraph.next<CompoundComponent>(archetype);
        archetype = archetypeGraph.next<ComplexComponent<bool>>(archetype);

        archetype = archetypeGraph.next<SimpleComponent>(makeArchetype<>());
        archetypeGraph.next<Transform>(archetype);
        archetypeGraph.next<ComplexComponent<float>>(archetype);

        auto supersets = archetypeGraph.getSupersets(makeArchetype<EmptyComponent>());
        CHECK(supersets.contains(makeArchetype<EmptyComponent>()));
        CHECK(supersets.contains(makeArchetype<EmptyComponent, SimpleComponent>()));
        CHECK(supersets.contains(makeArchetype<EmptyComponent, SimpleComponent, CompoundComponent>()));
        CHECK(supersets.contains(
                makeArchetype<EmptyComponent, SimpleComponent, CompoundComponent, ComplexComponent<bool>>()));
        CHECK_FALSE(supersets.contains(makeArchetype<>()));
        CHECK_FALSE(supersets.contains(makeArchetype<SimpleComponent>()));
        CHECK_FALSE(supersets.contains(makeArchetype<SimpleComponent, Transform>()));
        CHECK_FALSE(supersets.contains(makeArchetype<SimpleComponent, Transform, ComplexComponent<float>>()));
    }
}
