//
// Created by hiram on 6/6/22.
//

#pragma once

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
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
#include "RenderingSystem.hpp"

namespace citty::graphics {
    class GLFWRenderingSystem : public engine::System {
    public:
        explicit GLFWRenderingSystem(GLFWwindow *window);

        void init() override;

        void update() override;

        void render();

        std::size_t loadTexture(std::filesystem::path const &texturePath, TextureSettings settings);

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

        std::optional<std::size_t> loadAssimpTexture(aiMaterial *assimpMaterial, aiTextureType textureType);

        std::size_t loadAssimpMaterial(aiMaterial *assimpMaterial);

        std::size_t loadAssimpMesh(aiMesh *assimpMesh);

        std::unique_ptr<RenderingEngine> renderingEngine = nullptr;

        std::vector<GraphicsEntity> graphicEntities;
        std::timed_mutex graphicEntityMutex;

        std::vector<PointLightEntity> pointLightEntities;
        std::timed_mutex pointLightMutex;

        std::unordered_map<std::filesystem::path, std::size_t, CanonicalPathHash, EquivalentPathComparison> loadedTextures;

        std::queue<std::tuple<std::promise<std::size_t>, std::filesystem::path, TextureSettings>> textureLoadQueue;
        std::queue<std::pair<std::promise<std::size_t>, Material>> materialLoadQueue;
        std::queue<std::pair<std::promise<std::size_t>, Mesh>> meshLoadQueue;
        std::mutex loadMutex;

        std::vector<Model> models;

        GLFWwindow *window;

        std::size_t emptyTextureId = 0;
    };

} // graphics
