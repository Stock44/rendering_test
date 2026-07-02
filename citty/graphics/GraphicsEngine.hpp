//
// Created by hiram on 6/30/26.
//

#pragma once
#include <epoxy/gl.h>

#include <filesystem>
#include <mutex>
#include <vector>
#include <GLFW/glfw3.h>

#include "AssetLoader.hpp"
#include "GraphicsEntity.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "TextureSettings.hpp"

namespace citty::graphics
{
    class GraphicsEngine
    {
    public:
        GraphicsEngine(GLFWwindow* window);

        /**
         * Swaps the graphics entities with the given input graphic entities, allowing calling code to reuse
         * vectors by continuously swapping between two or more vectors.
         * @param entities The vector of graphics entities to swap with.
         */
        void swapGraphicEntities(std::vector<GraphicsEntity>& entities);

        void swapPointLightEntities(std::vector<PointLightEntity>& entities);

        void start();

        std::future<Renderer::TextureId> loadTexture(std::filesystem::path const& texturePath,
                                TextureSettings settings);

        std::future<Renderer::MaterialId> loadMaterial(Material const& material);

        std::future<Renderer::MeshId> loadMesh(Mesh const& mesh);

        static void windowChangeHandler(GLFWwindow* window, int width, int height);

        void onWindowSizeChange(int width, int height);

    private:
        void uploadGraphicsEntities();

        void uploadPointLightEntities();

        std::mutex graphicEntityMutex;
        std::vector<GraphicsEntity> graphicEntities;
        bool areGraphicsEntitiesDirty = false;

        std::vector<PointLightEntity> pointLightEntities;
        std::mutex pointLightMutex;
        bool arePointLightEntitiesDirty = false;

        GLFWwindow* window;

        std::unique_ptr<Renderer> renderer;

        std::unique_ptr<AssetLoader> assetLoader;
    };
} // graphics
