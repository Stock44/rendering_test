//
// Created by hiram on 6/30/26.
//

#pragma once
#include <cstddef>
#include <filesystem>
#include <future>
#include <queue>

#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "TextureSettings.hpp"

namespace citty::graphics
{
    struct CanonicalPathHash
    {
        std::hash<std::string> hasher{};

        /**
         * Obtains the hash for a given path by hashing its weakly-canonical
         * representation. Uses weakly_canonical (rather than canonical) so that
         * paths which don't exist on disk yet don't throw.
         * @param path the path to hash
         * @return the hash value
         */
        std::size_t operator()(std::filesystem::path const& path) const
        {
            return hasher(std::filesystem::weakly_canonical(path).string());
        }
    };

    struct EquivalentPathComparison
    {
        bool operator()(std::filesystem::path const& lhs,
                        std::filesystem::path const& rhs) const
        {
            return std::filesystem::weakly_canonical(lhs) ==
                std::filesystem::weakly_canonical(rhs);
        }
    };

    class AssetLoader
    {
    public:
        std::future<Renderer::TextureId> enqueueTextureLoad(std::filesystem::path const& texturePath,
                                TextureSettings settings);

        std::future<Renderer::MaterialId> enqueueMaterialLoad(Material const& material);

        std::future<Renderer::MeshId> enqueueMeshLoad(Mesh const& mesh);

        void loadPendingAssets(Renderer *renderer);
    private:
        // This map stores the already loaded texture by weakly-canonical path, to ensure that we don't load the same texture multiple times.
        std::unordered_map<std::filesystem::path, std::size_t, CanonicalPathHash,
                           EquivalentPathComparison>
        loadedTextures;
        // This queue stores the texture loading requests that are waiting to be processed.
        std::queue<std::tuple<std::promise<std::size_t>, std::filesystem::path,
                              TextureSettings>>
        textureLoadQueue;
        std::mutex textureLoadMutex;

        // Same but for materials
        std::queue<std::pair<std::promise<std::size_t>, Material>> materialLoadQueue;
        std::mutex materialLoadMutex;

        // And for meshes
        std::queue<std::pair<std::promise<std::size_t>, Mesh>> meshLoadQueue;
        std::mutex meshLoadMutex;

        std::vector<Model> models;
    };
}
