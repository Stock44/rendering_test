//
// Created by hiram on 4/11/23.
//

#pragma once

#include <citty/graphics/Texture.hpp>
#include <citty/graphics/Mesh.hpp>
#include <citty/graphics/Material.hpp>
#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/VertexArray.hpp>
#include <citty/graphics/ShaderProgram.hpp>
#include <citty/graphics/TextureSettings.hpp>
#include <mutex>
#include <citty/graphics/Texture2D.hpp>

namespace citty::graphics {

    struct MeshRecord {
        std::shared_ptr<Buffer<Eigen::Affine3f>> transformBuffer;
        std::vector<Eigen::Affine3f> transforms;
        VertexArray vertexArrayObject;
        std::size_t verticesOffset;
        std::size_t indicesOffset;
        std::size_t indicesSize;
    };

    struct GraphicsEntity {
        Eigen::Affine3f transform;
        std::size_t materialId{};
        std::size_t meshId{};

        [[nodiscard]] std::size_t key() const {
            return (materialId << 16) | meshId;
        }
    };

    class RenderingEngine {
    public:
        RenderingEngine();

        using MeshId = std::size_t;
        using TextureId = std::size_t;
        using MaterialId = std::size_t;

        void render();

        MeshId loadMesh(Mesh const &mesh);

        TextureId loadTexture(Image const &image, TextureSettings settings);

        MaterialId loadMaterial(Material const &material);

        bool textureIsLoaded(std::size_t textureId) const;

        void setGraphicsEntities(const std::vector<GraphicsEntity> &graphicsEntities);

        [[nodiscard]] const std::pair<int, int> &getViewportDimensions() const;

        void setViewportDimensions(int width, int height);

        [[nodiscard]] Eigen::Affine3f const &getView() const;

        void setView(Eigen::Affine3f const &newView);

        [[nodiscard]] Eigen::Projective3f const &getProjection() const;

        void setProjection(Eigen::Projective3f const &newProjection);

    private:
        void useMaterial(std::size_t materialId);

        struct RenderCommand {
            std::size_t instanceCount{};
            std::size_t offset{};
            std::size_t materialId{};
            std::size_t meshId{};
        };

        std::vector<RenderCommand> renderCommands;

        ShaderProgram shaderProgram;

        std::pair<int, int> viewportDimensions;

        Eigen::Affine3f view{};
        Eigen::Projective3f projection{};

        std::vector<MeshRecord> meshRecords;
        std::vector<Texture2D> materialTextures;
        std::vector<Material> materials;
        std::shared_ptr<Buffer<Vertex>> vertexBuffer = std::make_shared<Buffer<Vertex>>(BufferUsage::STATIC_DRAW);
        std::shared_ptr<Buffer<unsigned int>> indexBuffer = std::make_shared<Buffer<unsigned int>>(BufferUsage::STATIC_DRAW);
    };
} // graphics