file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.cpp"
)
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*\\.test\\.cpp$")

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.test.cpp"
)

add_library(citty_core OBJECT ${CORE_SOURCES})

target_compile_features(citty_core PUBLIC cxx_std_23)

target_include_directories(citty_core
  PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "${STB_INCLUDE_DIR}"
)

target_link_libraries(citty_core
  PUBLIC
    Eigen3::Eigen
    Boost::hana
    Boost::flyweight
    PkgConfig::EPOXY
  PRIVATE
    Boost::graph
    assimp::assimp
    pugixml::pugixml
)

add_executable(citty
  "${CMAKE_CURRENT_SOURCE_DIR}/citty.cpp"
  $<TARGET_OBJECTS:citty_core>
)

target_link_libraries(citty PRIVATE
  citty_core
  glfw
  assimp::assimp
  pugixml::pugixml
)

# citty-graphics-only.cpp defines main(), which conflicts with Catch2WithMain.
# The test binary contains only the test TUs + shared core.
add_executable(citty_tests
  ${TEST_SOURCES}
  $<TARGET_OBJECTS:citty_core>
)
target_link_libraries(citty_tests PRIVATE
  citty_core
  glfw
  assimp::assimp
  pugixml::pugixml
  Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(citty_tests)
