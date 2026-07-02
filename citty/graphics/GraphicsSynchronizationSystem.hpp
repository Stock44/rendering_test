//
// Created by hiram on 6/6/22.
//

#pragma once

#include <epoxy/gl.h>

#include <Eigen/Core>
#include <GLFW/glfw3.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/texture.h>
#include <citty/engine/System.hpp>
#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/Model.hpp>
#include <citty/graphics/Renderer.hpp>
#include <citty/graphics/Shader.hpp>
#include <citty/graphics/ShaderProgram.hpp>
#include <citty/graphics/VertexArray.hpp>
#include <citty/graphics/components/Camera.hpp>
#include <citty/graphics/components/Graphics.hpp>
#include <filesystem>
#include <future>
#include <mutex>

#include "GraphicsEngine.hpp"

namespace citty::graphics {

class GraphicsSynchronizationSystem : public engine::System {
public:
  explicit GraphicsSynchronizationSystem(GLFWwindow *window, GraphicsEngine *graphicsEngine);

  void init() override;

  void update() override;


  void handleGraphicsEntities();

  void handlePointLightEntities();


private:

  GLFWwindow *window;

  GraphicsEngine *graphicsEngine;
};

} // namespace citty::graphics
