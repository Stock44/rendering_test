//
// Created by hiram on 6/6/22.
//

#pragma once

#include <gtkmm/glarea.h>
#include <citty/engine/System.hpp>
#include <citty/graphics/components/Camera.hpp>
#include <citty/graphics/components/Graphics.hpp>
#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/VertexArray.hpp>
#include <citty/graphics/Shader.hpp>
#include <citty/graphics/ShaderProgram.hpp>
#include <citty/graphics/Model.hpp>
#include <mutex>
#include <citty/graphics/RenderingEngine.hpp>
#include <cmath>
#include <numbers>
#include <future>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/texture.h>
#include <filesystem>

namespace citty::graphics {
    struct CanonicalPathHash {
        /**
         * Obtains the hash for a given path by hashing its canonical representation.
         * @param path the path to hash
         * @return the hash value
         */
        std::size_t operator()(std::filesystem::path const &path) const {
            return std::hash<std::string>()(std::filesystem::canonical(path).string());
        }
    };

    struct EquivalentPathComparison {
        bool operator()(std::filesystem::path const &lhs, std::filesystem::path const &rhs) const {
            return std::filesystem::equivalent(lhs, rhs);
        }
    };

    class RenderingSystem : public engine::System {
    public:
        explicit RenderingSystem(Gtk::GLArea *glArea);

        void init() override;

        void update() override;

        void render();

        std::size_t loadTexture(std::filesystem::path const &texturePath, TextureSettings settings);

        std::size_t loadMaterial(Material const &material);

        std::size_t loadMesh(Mesh const &mesh);

        std::size_t loadModel(std::filesystem::path const &modelPath);

        engine::Entity buildModelInstance(std::size_t modelId);

        void handleGraphicsEntities();

    private:
        void uploadGraphicsEntities();

        void processLoadingQueues();

        std::optional<std::size_t> loadAssimpTexture(aiMaterial *assimpMaterial, aiTextureType textureType);

        std::size_t loadAssimpMaterial(aiMaterial *assimpMaterial);

        std::size_t loadAssimpMesh(aiMesh *assimpMesh);

        std::unique_ptr<RenderingEngine> renderingEngine = nullptr;

        std::vector<GraphicsEntity> graphicEntities;
        std::timed_mutex graphicEntityMutex;

        std::unordered_map<std::filesystem::path, std::size_t, CanonicalPathHash, EquivalentPathComparison> loadedTextures;

        std::queue<std::tuple<std::promise<std::size_t>, std::filesystem::path, TextureSettings>> textureLoadQueue;
        std::queue<std::pair<std::promise<std::size_t>, Material>> materialLoadQueue;
        std::queue<std::pair<std::promise<std::size_t>, Mesh>> meshLoadQueue;
        std::mutex loadMutex;

        std::vector<Model> models;

        std::size_t emptyTextureId;
    };

} // graphics
