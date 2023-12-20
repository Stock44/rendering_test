//
// Created by hiram on 6/6/22.
//

#include <iterator>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <citty/graphics/GLFWRenderingSystem.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/ShaderProgramBuilder.hpp>
#include <citty/graphics/Math.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace citty::graphics {
    GLFWRenderingSystem::GLFWRenderingSystem(GLFWwindow *window) : window(window) {
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, GLFWRenderingSystem::windowChangeHandler);

        glfwMakeContextCurrent(window);

        renderingEngine = std::make_unique<RenderingEngine>();
        renderingEngine->setViewpoint({0.0f, 0.0f, 0.0f},
                                      Eigen::Quaternionf::Identity());
        renderingEngine->setViewportDimensions(1920, 1080);
        renderingEngine->setProjection(
                perspectiveProjection(90.0f, static_cast<float>(1920) / static_cast<float>(1080), 5.0f, 50.0f));
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
        emptyTextureId = loadTexture("resources/no_texture.png",
                                     graphics::TextureSettings{});
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
        while (!textureLoadQueue.empty()) {
            std::scoped_lock lock{loadMutex};
            auto &[idPromise, texturePath, textureSettings] = textureLoadQueue.front();
            Image image{texturePath};
            std::size_t textureId = renderingEngine->loadTexture(image, textureSettings);
            idPromise.set_value(textureId);
            textureLoadQueue.pop();
        }

        while (!materialLoadQueue.empty()) {
            std::scoped_lock lock{loadMutex};
            auto &[idPromise, material] = materialLoadQueue.front();
            std::size_t materialId = renderingEngine->loadMaterial(material);
            idPromise.set_value(materialId);
            materialLoadQueue.pop();
        }

        while (!meshLoadQueue.empty()) {
            std::scoped_lock lock{loadMutex};
            auto &[idPromise, mesh] = meshLoadQueue.front();
            std::size_t meshId = renderingEngine->loadMesh(mesh);
            idPromise.set_value(meshId);
            meshLoadQueue.pop();
        }
    }

    std::size_t GLFWRenderingSystem::loadTexture(std::filesystem::path const &texturePath, TextureSettings settings) {
        if (loadedTextures.contains(texturePath)) {
            return loadedTextures.at(texturePath);
        }

        std::promise<std::size_t> idPromise;
        auto idFuture = idPromise.get_future();
        {
            std::scoped_lock lock{loadMutex};
            textureLoadQueue.emplace(std::move(idPromise), texturePath, settings);
        }
        auto textureId = idFuture.get();
        loadedTextures.try_emplace(texturePath, textureId);
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

        std::ranges::transform(components, std::back_inserter(graphicEntities), [](auto components) {
            auto const &[transform, graphic] = components;
            Eigen::Affine3f transformMatrix;
            if (transform.parent) {
                transformMatrix = Eigen::Affine3f::Identity();
                std::stack<Eigen::Affine3f> transformMatrices;
                transformMatrices.emplace(Eigen::Translation3f{transform.position} * transform.rotation *
                                          Eigen::AlignedScaling3f{transform.scale});
                std::optional<engine::Entity> currentEntity = transform.parent;
                while (currentEntity) {
                    engine::Transform const &currentTransform = currentEntity.value().getComponent<engine::Transform>();
                    transformMatrices.emplace(
                            Eigen::Translation3f{currentTransform.position} * currentTransform.rotation *
                            Eigen::AlignedScaling3f{currentTransform.scale});
                    if (!currentTransform.parent) break;
                    if (currentEntity == currentTransform.parent)
                        throw std::runtime_error("error: transform loop detected");
                    currentEntity = currentTransform.parent;
                }

                while (!transformMatrices.empty()) {
                    transformMatrix = transformMatrix * transformMatrices.top();
                    transformMatrices.pop();
                }
            } else {
                transformMatrix = Eigen::Translation3f(transform.position) * transform.rotation *
                                  Eigen::AlignedScaling3f{transform.scale};
            }
            return GraphicsEntity{transformMatrix, graphic.material, graphic.mesh};
        });
    }

    std::size_t GLFWRenderingSystem::loadModel(std::filesystem::path const &modelPath) {
        Assimp::Importer importer;
        aiScene const *scene = importer.ReadFile(modelPath.c_str(),
                                                 aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_JoinIdenticalVertices | aiProcess_FixInfacingNormals |
                                                 aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error("could not load model");
        }

        std::unordered_map<std::size_t, std::size_t> meshIdMap;
        std::unordered_map<std::size_t, std::size_t> materialIdMap;

        for (std::size_t modelMeshId = 0; modelMeshId < scene->mNumMeshes; modelMeshId++) {
            aiMesh *assimpMesh = scene->mMeshes[modelMeshId];
            auto meshId = loadAssimpMesh(assimpMesh);
            meshIdMap.try_emplace(modelMeshId, meshId);
        }

        for (std::size_t modelMaterialId = 0; modelMaterialId < scene->mNumMaterials; modelMaterialId++) {
            auto assimpMaterial = scene->mMaterials[modelMaterialId];
            auto materialId = loadAssimpMaterial(assimpMaterial);
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
                               {scaling.x,  scaling.y,  scaling.z}};

            for (std::size_t nodeMeshIdx = 0; nodeMeshIdx < assimpNode->mNumMeshes; nodeMeshIdx++) {
                auto assimpMeshId = assimpNode->mMeshes[nodeMeshIdx];
                auto meshId = meshIdMap.at(assimpMeshId);
                auto materialId = materialIdMap.at(scene->mMeshes[assimpMeshId]->mMaterialIndex);
                node->graphics.emplace_back(meshId, materialId);
            }

            for (std::size_t childIdx; childIdx < assimpNode->mNumChildren; childIdx++) {
                auto childAssimpNode = assimpNode->mChildren[childIdx];
                auto &childNode = node->childNodes.emplace_back(std::make_unique<Model>());
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
        rootEntity.addComponent<engine::Transform>();
        unexploredNodes.emplace(rootEntity, &rootNode);

        while (!unexploredNodes.empty()) {
            auto [entity, node] = unexploredNodes.front();
            unexploredNodes.pop();

            for (auto &graphics: node->graphics) {
                auto childEntity = newEntity();
                childEntity.addComponent<engine::Transform>();
                engine::Transform &childTransform = childEntity.getComponent<engine::Transform>();
                childTransform.parent = entity;
                childEntity.addComponent<Graphics>(graphics);
            }

            for (auto &childNode: node->childNodes) {
                auto childEntity = newEntity();
                childEntity.addComponent<engine::Transform>(node->transform);
                childEntity.getComponent<engine::Transform>().parent = entity;

                unexploredNodes.emplace(childEntity, childNode.get());
            }
        }

        return rootEntity;
    }

    std::optional<std::size_t> GLFWRenderingSystem::loadAssimpTexture(aiMaterial *material, aiTextureType textureType) {
        if (material->GetTextureCount(textureType) > 0) {
            aiString str;
            material->GetTexture(textureType, 0, &str);
            std::filesystem::path texturePath{str.C_Str()};
            return loadTexture(texturePath, TextureSettings{});
        }
        return {};
    }

    std::size_t GLFWRenderingSystem::loadAssimpMaterial(aiMaterial *assimpMaterial) {
        aiColor3D diffuseColor;
        aiColor3D specularColor;
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
        Eigen::Vector3f diffuse{diffuseColor.r, diffuseColor.g, diffuseColor.b};
        Eigen::Vector3f specular{specularColor.r, specularColor.g, specularColor.b};

        auto diffuseMap = loadAssimpTexture(assimpMaterial, aiTextureType_DIFFUSE);
        auto specularMap = loadAssimpTexture(assimpMaterial, aiTextureType_SPECULAR);
        auto heightMap = loadAssimpTexture(assimpMaterial, aiTextureType_HEIGHT);
        auto normalMap = loadAssimpTexture(assimpMaterial, aiTextureType_NORMALS);

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

        for (std::size_t vertexId = 0; vertexId < assimpMesh->mNumVertices; vertexId++) {
            auto position = assimpMesh->mVertices[vertexId];
            auto normal = assimpMesh->mNormals[vertexId];
            auto tangent = assimpMesh->mTangents[vertexId];
            auto bitangent = assimpMesh->mBitangents[vertexId];
            auto texCoords = aiVector3D{0.0f, 0.0f, 0.0f};
            if (assimpMesh->HasTextureCoords(0)) {
                texCoords = assimpMesh->mTextureCoords[0][vertexId];
            }
            Vertex vertex{
                    {position.x,  position.y,  position.z},
                    {normal.x,    normal.y,    normal.z},
                    {tangent.x,   tangent.y,   tangent.z},
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
        for (auto const &[transform, pointLight]: components) {
            Eigen::Vector4f position, color;
            position << transform.position, 1.0f;
            color << pointLight.color, 1.0f;
            pointLightEntities.emplace_back(position,
                                            color, pointLight.radius);
        }
    }

    void GLFWRenderingSystem::onWindowSizeChange(int width, int height) {
        renderingEngine->setViewportDimensions(width, height);
        renderingEngine->setProjection(
                perspectiveProjection(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.0f, 100.0f));
    }

    void GLFWRenderingSystem::windowChangeHandler(GLFWwindow *window, int width, int height) {
        static_cast<GLFWRenderingSystem *>(glfwGetWindowUserPointer(window))->onWindowSizeChange(width, height);
    }
}


