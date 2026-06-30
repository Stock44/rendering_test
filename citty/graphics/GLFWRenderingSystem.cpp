//
// Created by hiram on 6/6/22.
//

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/GLFWRenderingSystem.hpp>
#include <citty/graphics/Math.hpp>
#include <citty/graphics/ShaderProgramBuilder.hpp>
#include <iterator>
#include <unordered_set>

namespace {
constexpr float kFieldOfView = 45.0f;
constexpr float kNearPlane = 0.1f;
constexpr float kFarPlane = 100.0f;
} // namespace

namespace citty::graphics {
GLFWRenderingSystem::GLFWRenderingSystem(GLFWwindow *window) : window(window) {
  glfwSetWindowUserPointer(window, this);

  glfwSetFramebufferSizeCallback(window,
                                 GLFWRenderingSystem::windowChangeHandler);

  glfwMakeContextCurrent(window);

  int framebufferWidth = 0;
  int framebufferHeight = 0;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

  renderingEngine = std::make_unique<RenderingEngine>();
  renderingEngine->setViewpoint({0.0f, 0.0f, 0.0f},
                                Eigen::Quaternionf::Identity());
  renderingEngine->setViewportDimensions(
      static_cast<unsigned int>(framebufferWidth),
      static_cast<unsigned int>(framebufferHeight));
  renderingEngine->setProjection(perspectiveProjection(
      kFieldOfView,
      static_cast<float>(framebufferWidth) /
          static_cast<float>(framebufferHeight),
      kNearPlane, kFarPlane));
}

void GLFWRenderingSystem::start() {
  glfwMakeContextCurrent(window);
  while (!glfwWindowShouldClose(window)) {
    render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void GLFWRenderingSystem::init() {
  emptyTextureId =
      loadTexture("assets/no_texture.png", graphics::TextureSettings{});
}

void GLFWRenderingSystem::update() {
  handleGraphicsEntities();
  handlePointLightEntities();
}

void GLFWRenderingSystem::render() {
  processLoadingQueues();

  uploadGraphicsEntities();
  uploadPointLightEntities();

  renderingEngine->render();
}

void GLFWRenderingSystem::uploadGraphicsEntities() {
  using namespace std::chrono_literals;
  if (!graphicEntityMutex.try_lock_for(50ms)) {
    return;
  }
  std::lock_guard lock{graphicEntityMutex, std::adopt_lock};
  renderingEngine->setGraphicsEntities(graphicEntities);
}

void GLFWRenderingSystem::uploadPointLightEntities() {
  using namespace std::chrono_literals;
  if (!pointLightMutex.try_lock_for(50ms)) {
    return;
  }
  std::lock_guard lock{pointLightMutex, std::adopt_lock};
  renderingEngine->setPointLightEntities(pointLightEntities);
}

void GLFWRenderingSystem::processLoadingQueues() {
  while (true) {
    std::optional<std::tuple<std::promise<std::size_t>, std::filesystem::path,
                             TextureSettings>>
        item;
    {
      std::scoped_lock lock{loadMutex};
      if (textureLoadQueue.empty())
        break;
      item = std::move(textureLoadQueue.front());
      textureLoadQueue.pop();
    }
    auto &[idPromise, texturePath, textureSettings] = *item;
    try {
      Image image{texturePath};
      idPromise.set_value(renderingEngine->loadTexture(image, textureSettings));
    } catch (...) {
      idPromise.set_exception(std::current_exception());
    }
  }

  while (true) {
    std::optional<std::pair<std::promise<std::size_t>, Material>> item;
    {
      std::scoped_lock lock{loadMutex};
      if (materialLoadQueue.empty())
        break;
      item = std::move(materialLoadQueue.front());
      materialLoadQueue.pop();
    }
    auto &[idPromise, material] = *item;
    try {
      idPromise.set_value(renderingEngine->loadMaterial(material));
    } catch (...) {
      idPromise.set_exception(std::current_exception());
    }
  }

  while (true) {
    std::optional<std::pair<std::promise<std::size_t>, Mesh>> item;
    {
      std::scoped_lock lock{loadMutex};
      if (meshLoadQueue.empty())
        break;
      item = std::move(meshLoadQueue.front());
      meshLoadQueue.pop();
    }
    auto &[idPromise, mesh] = *item;
    try {
      idPromise.set_value(renderingEngine->loadMesh(mesh));
    } catch (...) {
      idPromise.set_exception(std::current_exception());
    }
  }
}

std::size_t
GLFWRenderingSystem::loadTexture(std::filesystem::path const &texturePath,
                                 TextureSettings settings) {
  {
    std::scoped_lock lock{loadMutex};
    if (auto it = loadedTextures.find(texturePath); it != loadedTextures.end()) {
      return it->second;
    }
  }

  std::promise<std::size_t> idPromise;
  auto idFuture = idPromise.get_future();
  {
    std::scoped_lock lock{loadMutex};
    textureLoadQueue.emplace(std::move(idPromise), texturePath, settings);
  }
  auto textureId = idFuture.get();
  {
    std::scoped_lock lock{loadMutex};
    loadedTextures.try_emplace(texturePath, textureId);
  }
  return textureId;
}

std::size_t GLFWRenderingSystem::loadMaterial(Material const &material) {
  std::promise<std::size_t> idPromise;
  auto idFuture = idPromise.get_future();
  {
    std::scoped_lock lock{loadMutex};
    materialLoadQueue.emplace(std::move(idPromise), material);
  }
  return idFuture.get();
}

std::size_t GLFWRenderingSystem::loadMesh(Mesh const &mesh) {
  std::promise<std::size_t> idPromise;
  auto idFuture = idPromise.get_future();
  {
    std::scoped_lock lock{loadMutex};
    meshLoadQueue.emplace(std::move(idPromise), mesh);
  }
  return idFuture.get();
}

void GLFWRenderingSystem::handleGraphicsEntities() {
  auto components = getComponents<engine::Transform, Graphics>();

  std::scoped_lock lock{graphicEntityMutex};
  graphicEntities.clear();

  std::ranges::transform(
      components, std::back_inserter(graphicEntities), [](auto components) {
        auto const &[transform, graphic] = components;
        Eigen::Affine3f transformMatrix;
        if (transform.parent) {
          transformMatrix = Eigen::Affine3f::Identity();
          std::stack<Eigen::Affine3f> transformMatrices;
          transformMatrices.emplace(Eigen::Translation3f{transform.position} *
                                    transform.rotation *
                                    Eigen::AlignedScaling3f{transform.scale});
          std::optional<engine::Entity> currentEntity = transform.parent;
          std::unordered_set<engine::Entity> visitedAncestors;
          while (currentEntity) {
            if (visitedAncestors.contains(currentEntity.value()))
              throw std::runtime_error("error: transform loop detected");
            visitedAncestors.insert(currentEntity.value());

            engine::Transform const &currentTransform =
                currentEntity.value().getComponent<engine::Transform>();
            transformMatrices.emplace(
                Eigen::Translation3f{currentTransform.position} *
                currentTransform.rotation *
                Eigen::AlignedScaling3f{currentTransform.scale});
            currentEntity = currentTransform.parent;
          }

          while (!transformMatrices.empty()) {
            transformMatrix = transformMatrix * transformMatrices.top();
            transformMatrices.pop();
          }
        } else {
          transformMatrix = Eigen::Translation3f(transform.position) *
                            transform.rotation *
                            Eigen::AlignedScaling3f{transform.scale};
        }
        return GraphicsEntity{transformMatrix, graphic.material, graphic.mesh};
      });
}

std::size_t
GLFWRenderingSystem::loadModel(std::filesystem::path const &modelPath) {
  Assimp::Importer importer;
  aiScene const *scene = importer.ReadFile(
      modelPath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                             aiProcess_JoinIdenticalVertices |
                             aiProcess_FixInfacingNormals |
                             aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw std::runtime_error("could not load model");
  }

  std::unordered_map<std::size_t, std::size_t> meshIdMap;
  std::unordered_map<std::size_t, std::size_t> materialIdMap;

  for (std::size_t modelMeshId = 0; modelMeshId < scene->mNumMeshes;
       modelMeshId++) {
    aiMesh *assimpMesh = scene->mMeshes[modelMeshId];
    auto meshId = loadAssimpMesh(assimpMesh);
    meshIdMap.try_emplace(modelMeshId, meshId);
  }

  for (std::size_t modelMaterialId = 0; modelMaterialId < scene->mNumMaterials;
       modelMaterialId++) {
    auto assimpMaterial = scene->mMaterials[modelMaterialId];
    auto materialId = loadAssimpMaterial(assimpMaterial, modelPath.parent_path());
    materialIdMap.try_emplace(modelMaterialId, materialId);
  }

  Model model;

  std::queue<std::pair<Model *, aiNode *>> unexploredNodes;
  unexploredNodes.emplace(&model, scene->mRootNode);

  while (!unexploredNodes.empty()) {
    auto [node, assimpNode] = unexploredNodes.front();
    unexploredNodes.pop();

    aiQuaternion rotation;
    aiVector3D position;
    aiVector3D scaling;
    assimpNode->mTransformation.Decompose(scaling, rotation, position);

    node->transform = {{rotation.w, rotation.x, rotation.y, rotation.z},
                       {position.x, position.y, position.z},
                       {scaling.x, scaling.y, scaling.z}};

    for (std::size_t nodeMeshIdx = 0; nodeMeshIdx < assimpNode->mNumMeshes;
         nodeMeshIdx++) {
      auto assimpMeshId = assimpNode->mMeshes[nodeMeshIdx];
      auto meshId = meshIdMap.at(assimpMeshId);
      auto materialId =
          materialIdMap.at(scene->mMeshes[assimpMeshId]->mMaterialIndex);
      node->graphics.emplace_back(meshId, materialId);
    }

    for (std::size_t childIdx = 0; childIdx < assimpNode->mNumChildren;
         childIdx++) {
      auto childAssimpNode = assimpNode->mChildren[childIdx];
      auto &childNode =
          node->childNodes.emplace_back(std::make_unique<Model>());
      unexploredNodes.emplace(childNode.get(), childAssimpNode);
    }
  }

  models.emplace_back(std::move(model));

  return models.size() - 1;
}

engine::Entity GLFWRenderingSystem::buildModelInstance(std::size_t modelId) {
  auto rootEntity = newEntity();
  auto &rootNode = models.at(modelId);

  std::queue<std::pair<engine::Entity, Model *>> unexploredNodes;
  rootEntity.addComponent<engine::Transform>(rootNode.transform);
  unexploredNodes.emplace(rootEntity, &rootNode);

  while (!unexploredNodes.empty()) {
    auto [entity, node] = unexploredNodes.front();
    unexploredNodes.pop();

    for (auto &graphics : node->graphics) {
      auto childEntity = newEntity();
      childEntity.addComponent<engine::Transform>();
      engine::Transform &childTransform =
          childEntity.getComponent<engine::Transform>();
      childTransform.parent = entity;
      childEntity.addComponent<Graphics>(graphics);
    }

    for (auto &childNode : node->childNodes) {
      auto childEntity = newEntity();
      childEntity.addComponent<engine::Transform>(childNode->transform);
      childEntity.getComponent<engine::Transform>().parent = entity;

      unexploredNodes.emplace(childEntity, childNode.get());
    }
  }

  return rootEntity;
}

std::optional<std::size_t>
GLFWRenderingSystem::loadAssimpTexture(aiMaterial *material,
                                       aiTextureType textureType,
                                       std::filesystem::path const &baseDir) {
  if (material->GetTextureCount(textureType) > 0) {
    aiString str;
    material->GetTexture(textureType, 0, &str);
    std::filesystem::path texturePath{str.C_Str()};
    if (!texturePath.empty() && texturePath.native().front() != '*') {
      texturePath = baseDir / texturePath;
    }
    return loadTexture(texturePath, TextureSettings{});
  }
  return {};
}

std::size_t
GLFWRenderingSystem::loadAssimpMaterial(aiMaterial *assimpMaterial,
                                        std::filesystem::path const &baseDir) {
  aiColor3D diffuseColor;
  aiColor3D specularColor;
  assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
  assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
  Eigen::Vector3f diffuse{diffuseColor.r, diffuseColor.g, diffuseColor.b};
  Eigen::Vector3f specular{specularColor.r, specularColor.g, specularColor.b};

  auto diffuseMap = loadAssimpTexture(assimpMaterial, aiTextureType_DIFFUSE, baseDir);
  auto specularMap = loadAssimpTexture(assimpMaterial, aiTextureType_SPECULAR, baseDir);
  auto heightMap = loadAssimpTexture(assimpMaterial, aiTextureType_HEIGHT, baseDir);
  auto normalMap = loadAssimpTexture(assimpMaterial, aiTextureType_NORMALS, baseDir);

  Material material{
      diffuse,
      specular,
      diffuseMap ? diffuseMap.value() : emptyTextureId,
      specularMap ? specularMap.value() : emptyTextureId,
      normalMap ? normalMap.value() : emptyTextureId,
      heightMap ? heightMap.value() : emptyTextureId,
  };

  auto materialId = loadMaterial(material);
  return materialId;
}

std::size_t GLFWRenderingSystem::loadAssimpMesh(aiMesh *assimpMesh) {
  Mesh mesh;

  for (std::size_t vertexId = 0; vertexId < assimpMesh->mNumVertices;
       vertexId++) {
    auto position = assimpMesh->mVertices[vertexId];
    auto normal = assimpMesh->mNormals[vertexId];
    auto tangent = aiVector3D{0.0f, 0.0f, 0.0f};
    auto bitangent = aiVector3D{0.0f, 0.0f, 0.0f};
    if (assimpMesh->HasTangentsAndBitangents()) {
      tangent = assimpMesh->mTangents[vertexId];
      bitangent = assimpMesh->mBitangents[vertexId];
    }
    auto texCoords = aiVector3D{0.0f, 0.0f, 0.0f};
    if (assimpMesh->HasTextureCoords(0)) {
      texCoords = assimpMesh->mTextureCoords[0][vertexId];
    }
    Vertex vertex{
        {position.x, position.y, position.z},
        {normal.x, normal.y, normal.z},
        {tangent.x, tangent.y, tangent.z},
        {bitangent.x, bitangent.y, bitangent.z},
        {texCoords.x, texCoords.y},
    };
    mesh.vertices.emplace_back(std::move(vertex));
  }

  for (std::size_t faceId = 0; faceId < assimpMesh->mNumFaces; faceId++) {
    aiFace face = assimpMesh->mFaces[faceId];
    for (std::size_t i = 0; i < face.mNumIndices; i++) {
      mesh.indices.push_back(face.mIndices[i]);
    }
  }

  return loadMesh(mesh);
}

void GLFWRenderingSystem::handlePointLightEntities() {
  auto components = getComponents<engine::Transform, PointLight>();

  std::scoped_lock lock{pointLightMutex};
  pointLightEntities.clear();
  for (auto const &[transform, pointLight] : components) {
    Eigen::Vector4f position, color;
    position << transform.position, 1.0f;
    color << pointLight.color, 1.0f;
    pointLightEntities.emplace_back(position, color, pointLight.radius);
  }
}

void GLFWRenderingSystem::onWindowSizeChange(int width, int height) {
  renderingEngine->setViewportDimensions(width, height);
  renderingEngine->setProjection(perspectiveProjection(
      kFieldOfView, static_cast<float>(width) / static_cast<float>(height),
      kNearPlane, kFarPlane));
}

void GLFWRenderingSystem::windowChangeHandler(GLFWwindow *window, int width,
                                              int height) {
  static_cast<GLFWRenderingSystem *>(glfwGetWindowUserPointer(window))
      ->onWindowSizeChange(width, height);
}
} // namespace citty::graphics
