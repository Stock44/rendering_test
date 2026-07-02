//
// Created by hiram on 6/30/26.
//

#pragma once
#include <future>
#include <vector>
#include <assimp/material.h>
#include <assimp/mesh.h>

#include "AssetLoader.hpp"
#include "GraphicsEngine.hpp"
#include "Model.hpp"
#include "citty/engine/Engine.hpp"

namespace citty::graphics
{
    Material importAssimpMaterial(const aiMaterial* assimpMaterial);

    Mesh importAssimpMesh(const aiMesh* assimpMesh);



    class AssimpModelLoader
    {
    public:
        AssimpModelLoader(GraphicsEngine* graphicsEngine, engine::EntityStore* entityStore);

        std::size_t loadModel(std::filesystem::path const& modelPath);

        engine::Entity buildModelInstance(std::size_t modelId);

    private:
        std::future<Renderer::TextureId>
        loadAssimpTexture(const aiMaterial* assimpMaterial, aiTextureType textureType,
                          std::filesystem::path const& baseDir);

        std::future<Renderer::MaterialId> loadAssimpMaterial(const aiMaterial* assimpMaterial,
                                       std::filesystem::path const& baseDir) ;

        std::future<Renderer::MeshId> loadAssimpMesh(const aiMesh* assimpMesh) const;

        std::vector<Model> models;

        GraphicsEngine *graphicsEngine;

        engine::EntityStore *entityStore;

        std::optional<Renderer::TextureId> defaultTextureId;
    };
} // citty::graphics
