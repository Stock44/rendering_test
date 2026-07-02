//
// Created by hiram on 6/30/26.
//

#include "AssimpModelLoader.hpp"

#include <algorithm>
#include <queue>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace citty::graphics
{
    AssimpModelLoader::AssimpModelLoader(GraphicsEngine* graphicsEngine, engine::EntityStore* entityStore) :
        graphicsEngine(graphicsEngine), entityStore(entityStore)
    {
    }

    std::size_t AssimpModelLoader::loadModel(
        std::filesystem::path const& modelPath)
    {
        Assimp::Importer importer;
        aiScene const* scene = importer.ReadFile(
            modelPath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_FixInfacingNormals |
            aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode)
        {
            throw std::runtime_error("could not load model");
        }

        std::unordered_map<std::size_t, std::size_t> meshIdMap;
        std::unordered_map<std::size_t, std::size_t> materialIdMap;

        for (std::size_t modelMeshId = 0; modelMeshId < scene->mNumMeshes;
             modelMeshId++)
        {
            aiMesh* assimpMesh = scene->mMeshes[modelMeshId];
            auto meshId = loadAssimpMesh(assimpMesh).get();
            meshIdMap.try_emplace(modelMeshId, meshId);
        }

        for (std::size_t modelMaterialId = 0; modelMaterialId < scene->mNumMaterials;
             modelMaterialId++)
        {
            auto assimpMaterial = scene->mMaterials[modelMaterialId];
            auto materialId =
                loadAssimpMaterial(assimpMaterial, modelPath.parent_path()).get();
            materialIdMap.try_emplace(modelMaterialId, materialId);
        }

        Model model;

        std::queue<std::pair<Model*, aiNode*>> unexploredNodes;
        unexploredNodes.emplace(&model, scene->mRootNode);

        while (!unexploredNodes.empty())
        {
            auto [node, assimpNode] = unexploredNodes.front();
            unexploredNodes.pop();

            aiQuaternion rotation;
            aiVector3D position;
            aiVector3D scaling;
            assimpNode->mTransformation.Decompose(scaling, rotation, position);

            node->transform = {
                {rotation.w, rotation.x, rotation.y, rotation.z},
                {position.x, position.y, position.z},
                {scaling.x, scaling.y, scaling.z}
            };

            for (std::size_t nodeMeshIdx = 0; nodeMeshIdx < assimpNode->mNumMeshes;
                 nodeMeshIdx++)
            {
                auto assimpMeshId = assimpNode->mMeshes[nodeMeshIdx];
                auto meshId = meshIdMap.at(assimpMeshId);
                auto materialId =
                    materialIdMap.at(scene->mMeshes[assimpMeshId]->mMaterialIndex);
                node->graphics.emplace_back(meshId, materialId);
            }

            for (std::size_t childIdx = 0; childIdx < assimpNode->mNumChildren;
                 childIdx++)
            {
                auto childAssimpNode = assimpNode->mChildren[childIdx];
                auto& childNode =
                    node->childNodes.emplace_back(std::make_unique<Model>());
                unexploredNodes.emplace(childNode.get(), childAssimpNode);
            }
        }

        models.emplace_back(std::move(model));

        return models.size() - 1;
    }


    std::future<Renderer::TextureId> AssimpModelLoader::loadAssimpTexture(
        const aiMaterial* material, const aiTextureType textureType,
        std::filesystem::path const& baseDir)
    {
        if (material->GetTextureCount(textureType) > 0)
        {
            aiString str;
            material->GetTexture(textureType, 0, &str);
            // Some .mtl files (e.g. Sponza) reference textures with Windows-style
            // backslashes, which std::filesystem::path won't treat as separators
            // on Linux, so normalize them before resolving the path.
            std::string rawPath = str.C_Str();
            std::ranges::replace(rawPath, '\\', '/');
            std::filesystem::path texturePath{rawPath};
            if (!texturePath.empty() && texturePath.native().front() != '*')
            {
                texturePath = baseDir / texturePath;
            }
            return graphicsEngine->loadTexture(texturePath, TextureSettings{});
        }

        if (!defaultTextureId.has_value())
        {
            defaultTextureId = {
                graphicsEngine->loadTexture(std::filesystem::path{"assets/no_texture.png"}, TextureSettings{}).get()
            };
        }

        auto promise = std::promise<Renderer::TextureId>();

        promise.set_value(defaultTextureId.value());

        return promise.get_future();
    }

    std::future<Renderer::MaterialId> AssimpModelLoader::loadAssimpMaterial(
        const aiMaterial* assimpMaterial, std::filesystem::path const& baseDir)
    {
        aiColor3D diffuseColor;
        aiColor3D specularColor;
        float shininess = 32.0f;
        assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
        assimpMaterial->Get(AI_MATKEY_SHININESS, shininess);
        Eigen::Vector3f diffuse{diffuseColor.r, diffuseColor.g, diffuseColor.b};
        Eigen::Vector3f specular{specularColor.r, specularColor.g, specularColor.b};

        auto diffuseMap =
            loadAssimpTexture(assimpMaterial, aiTextureType_DIFFUSE, baseDir);
        auto specularMap =
            loadAssimpTexture(assimpMaterial, aiTextureType_SPECULAR, baseDir);
        auto heightMap =
            loadAssimpTexture(assimpMaterial, aiTextureType_HEIGHT, baseDir);
        auto normalMap =
            loadAssimpTexture(assimpMaterial, aiTextureType_NORMALS, baseDir);

        const auto diffuseMapId = diffuseMap.get();
        const auto specularMapId = specularMap.get();
        const auto heightMapId = heightMap.get();
        const auto normalMapId = normalMap.get();

        const Material material{
            diffuse,
            specular,
            diffuseMapId,
            specularMapId,
            normalMapId,
            heightMapId,
            shininess,
        };

        return graphicsEngine->loadMaterial(material);
    }

    std::future<Renderer::MeshId> AssimpModelLoader::loadAssimpMesh(const aiMesh* assimpMesh) const
    {
        Mesh mesh;

        for (std::size_t vertexId = 0; vertexId < assimpMesh->mNumVertices;
             vertexId++)
        {
            auto position = assimpMesh->mVertices[vertexId];
            auto normal = assimpMesh->mNormals[vertexId];
            auto tangent = aiVector3D{0.0f, 0.0f, 0.0f};
            auto bitangent = aiVector3D{0.0f, 0.0f, 0.0f};
            if (assimpMesh->HasTangentsAndBitangents())
            {
                tangent = assimpMesh->mTangents[vertexId];
                bitangent = assimpMesh->mBitangents[vertexId];
            }
            auto texCoords = aiVector3D{0.0f, 0.0f, 0.0f};
            if (assimpMesh->HasTextureCoords(0))
            {
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

        for (std::size_t faceId = 0; faceId < assimpMesh->mNumFaces; faceId++)
        {
            aiFace face = assimpMesh->mFaces[faceId];
            for (std::size_t i = 0; i < face.mNumIndices; i++)
            {
                mesh.indices.push_back(face.mIndices[i]);
            }
        }

        return graphicsEngine->loadMesh(mesh);
    }


    engine::Entity
    AssimpModelLoader::buildModelInstance(std::size_t modelId)
    {
        auto rootEntity = entityStore->newEntity();
        auto& rootNode = models.at(modelId);

        std::queue<std::pair<engine::Entity, Model*>> unexploredNodes;
        rootEntity.addComponent<engine::Transform>(rootNode.transform);
        unexploredNodes.emplace(rootEntity, &rootNode);

        while (!unexploredNodes.empty())
        {
            auto [entity, node] = unexploredNodes.front();
            unexploredNodes.pop();

            for (auto& graphics : node->graphics)
            {
                auto childEntity = entityStore->newEntity();
                childEntity.addComponent<engine::Transform>();
                auto& childTransform =
                    childEntity.getComponent<engine::Transform>();
                childTransform.parent = entity;
                childEntity.addComponent<Graphics>(graphics);
            }

            for (auto& childNode : node->childNodes)
            {
                auto childEntity = entityStore->newEntity();
                childEntity.addComponent<engine::Transform>(childNode->transform);
                childEntity.getComponent<engine::Transform>().parent = entity;

                unexploredNodes.emplace(childEntity, childNode.get());
            }
        }

        return rootEntity;
    }
} // citty::graphics
