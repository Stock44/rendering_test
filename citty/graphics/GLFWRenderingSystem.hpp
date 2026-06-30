//
// Created by hiram on 6/6/22.
//

#pragma once

#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/texture.h>
#include <citty/engine/System.hpp>
#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/Model.hpp>
#include <citty/graphics/RenderingEngine.hpp>
#include <citty/graphics/Shader.hpp>
#include <citty/graphics/ShaderProgram.hpp>
#include <citty/graphics/VertexArray.hpp>
#include <citty/graphics/components/Camera.hpp>
#include <citty/graphics/components/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <future>
#include <mutex>
#include <numbers>

namespace citty::graphics {

struct CanonicalPathHash {
  std::hash<std::string> hasher{};

  /**
   * Obtains the hash for a given path by hashing its weakly-canonical
   * representation. Uses weakly_canonical (rather than canonical) so that
   * paths which don't exist on disk yet don't throw.
   * @param path the path to hash
   * @return the hash value
   */
  std::size_t operator()(std::filesystem::path const &path) const {
    return hasher(std::filesystem::weakly_canonical(path).string());
  }
};

struct EquivalentPathComparison {
  bool operator()(std::filesystem::path const &lhs,
                  std::filesystem::path const &rhs) const {
    return std::filesystem::weakly_canonical(lhs) ==
           std::filesystem::weakly_canonical(rhs);
  }
};

class GLFWRenderingSystem : public engine::System {
public:
  explicit GLFWRenderingSystem(GLFWwindow *window);

  void init() override;

  void update() override;

  void render();

  std::size_t loadTexture(std::filesystem::path const &texturePath,
                          TextureSettings settings);

  std::size_t loadMaterial(Material const &material);

  std::size_t loadMesh(Mesh const &mesh);

  std::size_t loadModel(std::filesystem::path const &modelPath);

  engine::Entity buildModelInstance(std::size_t modelId);

  void handleGraphicsEntities();

  void handlePointLightEntities();

  void start();

  static void windowChangeHandler(GLFWwindow *window, int width, int height);

  void onWindowSizeChange(int width, int height);

private:
  void uploadGraphicsEntities();

  void uploadPointLightEntities();

  void processLoadingQueues();

  std::optional<std::size_t> loadAssimpTexture(
      aiMaterial *assimpMaterial, aiTextureType textureType,
      std::filesystem::path const &baseDir);

  std::size_t loadAssimpMaterial(aiMaterial *assimpMaterial,
                                 std::filesystem::path const &baseDir);

  std::size_t loadAssimpMesh(aiMesh *assimpMesh);

  std::unique_ptr<RenderingEngine> renderingEngine = nullptr;

  std::vector<GraphicsEntity> graphicEntities;
  std::mutex graphicEntityMutex;
  bool graphicEntitiesDirty = false;

  std::vector<PointLightEntity> pointLightEntities;
  std::mutex pointLightMutex;
  bool pointLightEntitiesDirty = false;

  std::unordered_map<std::filesystem::path, std::size_t, CanonicalPathHash,
                     EquivalentPathComparison>
      loadedTextures;

  std::queue<std::tuple<std::promise<std::size_t>, std::filesystem::path,
                        TextureSettings>>
      textureLoadQueue;
  std::queue<std::pair<std::promise<std::size_t>, Material>> materialLoadQueue;
  std::queue<std::pair<std::promise<std::size_t>, Mesh>> meshLoadQueue;
  std::mutex loadMutex;

  std::vector<Model> models;

  GLFWwindow *window;

  std::size_t emptyTextureId = 0;
};

} // namespace citty::graphics
