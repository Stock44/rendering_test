cmake_minimum_required(VERSION 3.25)

project(citty VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EPOXY REQUIRED IMPORTED_TARGET epoxy>=1.5.10)

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(pugixml REQUIRED)
find_package(boost_hana      CONFIG REQUIRED)
find_package(boost_flyweight CONFIG REQUIRED)
find_package(boost_graph     CONFIG REQUIRED)
find_package(Catch2        REQUIRED)
find_package(glfw3           REQUIRED)
find_package(assimp          REQUIRED)
find_path(STB_INCLUDE_DIR stb_image.h REQUIRED)

add_subdirectory(citty)
