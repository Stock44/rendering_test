#include <catch2/catch_test_macros.hpp>
#include <citty/graphics/GraphicsSynchronizationSystem.hpp>
#include <filesystem>

TEST_CASE("path hashing/comparison does not throw on nonexistent paths",
          "[GLFWRenderingSystem]") {
  using namespace citty::graphics;

  std::filesystem::path missing{"this/path/does/not/exist.png"};

  SECTION("hashing a nonexistent path does not throw") {
    CanonicalPathHash hasher;
    REQUIRE_NOTHROW(hasher(missing));
  }

  SECTION("comparing nonexistent paths does not throw") {
    EquivalentPathComparison comparison;
    REQUIRE_NOTHROW(comparison(missing, missing));
    REQUIRE(comparison(missing, missing));
    REQUIRE_FALSE(
        comparison(missing, std::filesystem::path{"other/missing.png"}));
  }
}
