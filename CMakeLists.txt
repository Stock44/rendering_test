cmake_minimum_required(VERSION 3.0)
project(cityy VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(GLFW3 REQUIRED)

set(SOURCES src/main.cpp src/graphics/Shader.cpp src/graphics/GraphicsEngine.cpp src/graphics/GraphicsEngine.h src/graphics/Camera.cpp src/graphics/Camera.h src/graphics/types.h src/graphics/buffers/VertexBuffer.h src/graphics/Model.h src/graphics/Object.h src/graphics/buffers/VertexBuffer.cpp src/graphics/buffers/VertexBuffer.h src/graphics/Model.cpp src/graphics/Model.h src/graphics/VertexArray.cpp src/graphics/VertexArray.h src/graphics/Object.cpp src/graphics/Object.h src/graphics/VertexArrayBuilder.cpp src/graphics/VertexArrayBuilder.h src/graphics/buffers/Buffer.cpp src/graphics/buffers/Buffer.h src/graphics/buffers/ModelMatBuffer.cpp src/graphics/buffers/ModelMatBuffer.h src/graphics/buffers/IndexBuffer.cpp src/graphics/buffers/IndexBuffer.h src/Window.cpp src/Window.h src/map/Node.cpp src/map/Node.h src/map/Way.cpp src/map/Way.h src/map/Highway.cpp src/map/Highway.h src/map/TransitNetwork.cpp src/map/TransitNetwork.h src/map/MapXMLTree.cpp src/map/MapXMLTree.h src/map/utils.h src/map/types.h src/graphics/buffers/ColorVertexBuffer.cpp src/graphics/buffers/ColorVertexBuffer.h src/map/HighwayType.h src/map/TransitNetworkRenderer.cpp src/map/TransitNetworkRenderer.h)

add_executable(cityy ${SOURCES})
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} glfw glm glad stb_image rapidxml)

