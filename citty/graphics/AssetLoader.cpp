//
// Created by hiram on 6/30/26.
//

#include "AssetLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "GraphicsEngine.hpp"

namespace citty::graphics
{
    std::future<Renderer::TextureId> AssetLoader::enqueueTextureLoad(
        std::filesystem::path const& texturePath, TextureSettings settings)
    {
        // This whole function is protected by a mutex to prevent the renderer thread from attempting to load
        // textures at the same time we are adding new textures.
        std::scoped_lock lock{textureLoadMutex};

        // Set up a promise and future to handle the texture loading asynchronously, in a separate renderer thread.
        std::promise<Renderer::TextureId> textureIdPromise;
        auto textureIdFuture = textureIdPromise.get_future();

        textureLoadQueue.emplace(std::move(textureIdPromise), texturePath, settings);

        return textureIdFuture;
    }

    std::future<Renderer::MaterialId> AssetLoader::enqueueMaterialLoad(citty::graphics::Material const& material)
    {
        // This function is protected by a mutex to prevent the renderer thread from attempting to load materials at the
        // same time we are adding new materials.
        std::scoped_lock lock{materialLoadMutex};

        // Handle the material loading asynchronously, in a separate renderer thread.
        std::promise<std::size_t> idPromise;
        auto idFuture = idPromise.get_future();
        materialLoadQueue.emplace(std::move(idPromise), material);

        return idFuture;
    }

    std::future<Renderer::MeshId> AssetLoader::enqueueMeshLoad(citty::graphics::Mesh const& mesh)
    {
        // Protected by a mutex to prevent the renderer thread from loading the mesh at the same time we are adding new meshes.
        std::scoped_lock lock{meshLoadMutex};

        std::promise<std::size_t> idPromise;
        auto idFuture = idPromise.get_future();
        meshLoadQueue.emplace(std::move(idPromise), mesh);

        return idFuture;
    }

    void AssetLoader::loadPendingAssets(Renderer *renderer) {
        if (textureLoadMutex.try_lock())
        {
            std::scoped_lock lock{std::adopt_lock, textureLoadMutex};
            while (!textureLoadQueue.empty())
            {

                auto &item = textureLoadQueue.front();
                auto &[idPromise, texturePath, textureSettings] = item;

                try {
                    Image image{texturePath};
                    idPromise.set_value(renderer->loadTexture(image, textureSettings));
                } catch (...) {
                    idPromise.set_exception(std::current_exception());
                }

                textureLoadQueue.pop();
            }
        }

        if (materialLoadMutex.try_lock())
        {
            std::scoped_lock lock{std::adopt_lock, materialLoadMutex};

            while (!materialLoadQueue.empty())
            {
                auto &item = materialLoadQueue.front();
                auto &[idPromise, material] = item;
                try {
                    idPromise.set_value(renderer->loadMaterial(material));
                } catch (...) {
                    idPromise.set_exception(std::current_exception());
                }
                materialLoadQueue.pop();
            }
        }

        if (meshLoadMutex.try_lock())
        {
            std::scoped_lock lock{std::adopt_lock, meshLoadMutex};

            while (!meshLoadQueue.empty())
            {
                auto &item = meshLoadQueue.front();
                auto &[idPromise, mesh] = item;

                try {
                    idPromise.set_value(renderer->loadMesh(mesh));
                } catch (...) {
                    idPromise.set_exception(std::current_exception());
                }
                meshLoadQueue.pop();
            }
        }
    }

}
