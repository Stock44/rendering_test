cmake_minimum_required(VERSION 3.0)
project(cityy VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(pugixml REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(GLFW3 REQUIRED)

set(SOURCES src/main.cpp src/graphics/Shader.cpp src/graphics/buffers/VertexBuffer.h src/graphics/buffers/VertexBuffer.cpp src/graphics/buffers/VertexBuffer.h src/graphics/VertexArray.cpp src/graphics/VertexArray.h src/graphics/buffers/Buffer.cpp src/graphics/buffers/Buffer.h src/graphics/buffers/ModelMatBuffer.cpp src/graphics/buffers/ModelMatBuffer.h src/graphics/buffers/IndexBuffer.cpp src/graphics/buffers/IndexBuffer.h src/Window.cpp src/Window.h src/graphics/buffers/ColorVertexBuffer.cpp src/graphics/buffers/ColorVertexBuffer.h src/engine/ComponentStore.h src/engine/ComponentManager.cpp src/engine/ComponentManager.h src/engine/Entity.h src/engine/System.h src/engine/Engine.cpp src/engine/Engine.h src/engine/EntityManager.cpp src/engine/EntityManager.h src/graphics/components/Mesh.h src/graphics/components/Vertex.h src/graphics/RenderingSystem.cpp src/graphics/RenderingSystem.h src/components/Transform.h src/graphics/components/Camera.h src/graphics/components/MeshRef.h src/graphics/components/Color.h src/graphics/OpenGlError.cpp src/graphics/OpenGlError.h src/input/InputSystem.cpp src/input/InputSystem.h src/map/components/Road.h src/map/components/Node.h src/map/loadXMLMap.h src/map/MapRenderingSystem.cpp src/map/MapRenderingSystem.h)


add_executable(cityy ${SOURCES})
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} glfw glm glad stb_image pugixml)

