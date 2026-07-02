//
// Created by hiram on 6/30/26.
//

#include "GraphicsEngine.hpp"

#include <future>
#include <queue>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <GLFW/glfw3.h>

#include "Math.hpp"
#include "Model.hpp"

namespace
{
    constexpr float kFieldOfView = 45.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 100.0f;
} // namespace

namespace citty::graphics
{
    GraphicsEngine::GraphicsEngine(GLFWwindow* window): window{window}
    {
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(
            window, GraphicsEngine::windowChangeHandler);

        glfwMakeContextCurrent(window);

        renderer = std::make_unique<Renderer>();
        assetLoader = std::make_unique<AssetLoader>();

        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        renderer->setViewpoint({0.0f, 0.0f, 0.0f},
                               Eigen::Quaternionf::Identity());
        renderer->setViewportDimensions(
            static_cast<unsigned int>(framebufferWidth),
            static_cast<unsigned int>(framebufferHeight));
        renderer->setProjection(
            perspectiveProjection(kFieldOfView,
                                  static_cast<float>(framebufferWidth) /
                                  static_cast<float>(framebufferHeight),
                                  kNearPlane, kFarPlane));
    }

    void GraphicsEngine::swapGraphicEntities(std::vector<GraphicsEntity>& entities)
    {
        std::scoped_lock lock(graphicEntityMutex);

        std::swap(entities, graphicEntities);

        areGraphicsEntitiesDirty = true;
    }

    void GraphicsEngine::swapPointLightEntities(std::vector<PointLightEntity>& entities)
    {
        std::scoped_lock lock(pointLightMutex);

        std::swap(entities, pointLightEntities);

        arePointLightEntitiesDirty = true;
    }

    void GraphicsEngine::start()
    {
        glfwMakeContextCurrent(window);
        while (!glfwWindowShouldClose(window))
        {
            assetLoader->loadPendingAssets(renderer.get());

            uploadGraphicsEntities();
            uploadPointLightEntities();

            renderer->render();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    std::future<Renderer::TextureId> GraphicsEngine::loadTexture(std::filesystem::path const& texturePath, TextureSettings settings)
    {
        return assetLoader->enqueueTextureLoad(texturePath, settings);
    }

    void GraphicsEngine::onWindowSizeChange(int width, int height)
    {
        renderer->setViewportDimensions(width, height);
        renderer->setProjection(perspectiveProjection(
            kFieldOfView, static_cast<float>(width) / static_cast<float>(height),
            kNearPlane, kFarPlane));
    }

    std::future<Renderer::MaterialId> GraphicsEngine::loadMaterial(Material const& material)
    {
        return assetLoader->enqueueMaterialLoad(material);
    }

    std::future<Renderer::MeshId> GraphicsEngine::loadMesh(Mesh const& mesh)
    {
        return assetLoader->enqueueMeshLoad(mesh);
    }

    void GraphicsEngine::windowChangeHandler(GLFWwindow* window,
                                             int width, int height)
    {
        static_cast<GraphicsEngine*>(glfwGetWindowUserPointer(window))
            ->onWindowSizeChange(width, height);
    }


    void GraphicsEngine::uploadGraphicsEntities()
    {
        std::scoped_lock lock{graphicEntityMutex};
        if (!areGraphicsEntitiesDirty)
        {
            return;
        }
        areGraphicsEntitiesDirty = false;
        renderer->setGraphicsEntities(graphicEntities);
    }

    void GraphicsEngine::uploadPointLightEntities()
    {
        std::vector<PointLightEntity> entitiesToUpload;
        {
            std::scoped_lock lock{pointLightMutex};
            if (!arePointLightEntitiesDirty)
            {
                return;
            }
            renderer->setPointLightEntities(pointLightEntities);
            arePointLightEntitiesDirty = false;
        }
    }
} // graphics
