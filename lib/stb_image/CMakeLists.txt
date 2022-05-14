cmake_minimum_required(VERSION 3.18)

project(stb_image LANGUAGES C VERSION 2.27)

add_library(stb_image stb_image.c)

add_custom_command(
    OUTPUT stb_image.h
    COMMAND ${PROJECT_SOURCE_DIR}/download_latest.sh
)
add_custom_target(Download_stb_image DEPENDS stb_image.h)
add_dependencies(stb_image Download_stb_image)

target_include_directories(stb_image PUBLIC ${PROJECT_BINARY_DIR})
