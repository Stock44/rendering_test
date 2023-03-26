#include <catch2/catch_test_macros.hpp>
#include <citty/engine/ArchetypeRecord.hpp>
#include <citty/engine/components/TestComponents.hpp>
#include "citty/engine/components/Transform.hpp"

TEST_CASE ("constructs valid derived archetype records", "[ArchetypeRecord]") {
    using namespace engine;
    ArchetypeRecord record;

    REQUIRE_NOTHROW(record = record.constructDerivedRecord<SimpleComponent>());
    REQUIRE_NOTHROW(record = record.constructDerivedRecord<CompoundComponent>());
    REQUIRE_NOTHROW(record = record.constructDerivedRecord<EmptyComponent>());
    REQUIRE_NOTHROW(record = record.constructDerivedRecord<ComplexComponent<float>>());

    REQUIRE_THROWS(record.constructDerivedRecord<SimpleComponent>());
    REQUIRE_THROWS(record.constructDerivedRecord<ComplexComponent<float>>());
    REQUIRE_THROWS(record.constructDerivedRecord<EmptyComponent>());
    REQUIRE_THROWS(record.constructDerivedRecord<CompoundComponent>());

    REQUIRE_NOTHROW(record.constructDerivedRecord<Transform>());
}

TEST_CASE("can move entities and its components with valid record transitions", "[ArchetypeRecord]") {
    using namespace engine;
    ArchetypeRecord empty;
    ArchetypeRecord first = empty.constructDerivedRecord<SimpleComponent>();
    ArchetypeRecord second = first.constructDerivedRecord<Transform>();

    Entity entity = 0;
    Entity entity2 = 1;
    REQUIRE_NOTHROW(empty.add(entity));
    REQUIRE_NOTHROW(empty.add(entity2));

    REQUIRE_NOTHROW(empty.moveToNextArchetype<SimpleComponent>(entity, first, SimpleComponent{1.0f, 1, false}));
    REQUIRE_NOTHROW(empty.moveToNextArchetype<SimpleComponent>(entity2, first, SimpleComponent{2.0f, 21, true}));

    REQUIRE_NOTHROW(first.moveToNextArchetype<Transform>(entity, second));
    REQUIRE_NOTHROW(first.moveToNextArchetype<Transform>(entity2, second));

    SECTION("throws with invalid transitions") {
        REQUIRE_THROWS(first.moveToNextArchetype<CompoundComponent>(entity, first));
        REQUIRE_THROWS(second.moveToPrevArchetype<Transform>(entity, empty));
        REQUIRE_THROWS(second.moveToPrevArchetype<SimpleComponent>(entity, first));
    }

    REQUIRE_NOTHROW(second.moveToPrevArchetype<Transform>(entity, first));
    REQUIRE_NOTHROW(first.moveToPrevArchetype<SimpleComponent>(entity, empty));
    REQUIRE_NOTHROW(second.moveToPrevArchetype<Transform>(entity2, first));
    REQUIRE_NOTHROW(first.moveToPrevArchetype<SimpleComponent>(entity2, empty));
}

TEST_CASE("can access entity components", "[ArchetypeRecord]") {
    using namespace engine;
    ArchetypeRecord empty;
    ArchetypeRecord first = empty.constructDerivedRecord<SimpleComponent>();
    for (Entity i = 0; i < 30; i++) {
        REQUIRE_NOTHROW(empty.add(i));
    }

    for (Entity i = 0; i < 30; i++) {
        REQUIRE_NOTHROW(empty.moveToNextArchetype<SimpleComponent>(i, first, SimpleComponent{static_cast<float>(i),
                                                                                             static_cast<int>(i),
                                                                                             i % 2 == 0}));
    }

    for (Entity i = 0; i < 30; i++) {
        REQUIRE(first.get<SimpleComponent>(i) ==
                SimpleComponent{static_cast<float>(i), static_cast<int>(i), i % 2 == 0});
    }
}