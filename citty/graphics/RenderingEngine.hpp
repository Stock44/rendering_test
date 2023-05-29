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
#include <citty/graphics/Framebuffer.hpp>
#include <citty/graphics/Renderbuffer.hpp>
#include <citty/graphics/components/PointLight.hpp>
#include <citty/graphics/PointLightEntity.hpp>
#include <citty/graphics/GraphicsEntity.hpp>

namespace citty::graphics {

    struct MeshRecord {
        std::shared_ptr<Buffer<Eigen::Affine3f>> transformBuffer;
        std::vector<Eigen::Affine3f> transforms;
        VertexArray vertexArrayObject;
        std::size_t verticesOffset;
        std::size_t indicesOffset;
        std::size_t indicesSize;
    };

    struct QuadVertex {
        Eigen::Vector3f position;
        Eigen::Vector2f texCoords;
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

        void setGraphicsEntities(std::span<GraphicsEntity> graphicsEntities);

        void setPointLightEntities(std::span<PointLightEntity> pointLightEntities);

        [[nodiscard]] const std::pair<unsigned int, unsigned int> &getViewportDimensions() const;

        void setViewportDimensions(unsigned int width, unsigned int height);

        void setViewpoint(Eigen::Vector3f const &position, Eigen::Quaternionf const &rotation);

        [[nodiscard]] Eigen::Projective3f const &getProjection() const;

        void setProjection(Eigen::Projective3f const &newProjection);

    private:
        void useMaterial(std::size_t materialId);

        void issueDrawCommands();

        struct RenderCommand {
            std::size_t instanceCount{};
            std::size_t offset{};
            std::size_t materialId{};
            std::size_t meshId{};
        };

        std::vector<RenderCommand> renderCommands;

        ShaderProgram depthShaderProgram;

        ShaderProgram lightCullingShaderProgram;
        unsigned int lightCullingWorkgroupsX = 0;
        unsigned int lightCullingWorkgroupsY = 0;
        std::size_t numberOfTiles = 0;

        ShaderProgram lightAccumulationShaderProgram;
        ShaderProgram hdrShaderProgram;

        std::pair<unsigned int, unsigned int> viewportDimensions;

        Eigen::Vector3f viewPosition{};
        Eigen::Affine3f view{};
        Eigen::Projective3f projection{};

        // Lights
        unsigned int pointLightCount = 0;
        std::shared_ptr<Buffer<PointLightEntity>> pointLightsBuffer = std::make_shared<Buffer<PointLightEntity>>(
                BufferUsage::STREAM_DRAW);
        std::shared_ptr<Buffer<int>> visiblePointLightIndexBuffer = std::make_shared<Buffer<int>>(
                BufferUsage::STREAM_DRAW);

        // Depth
        Framebuffer depthFramebuffer;
        std::shared_ptr<Texture2D> depthTexture = nullptr;

        // HDR
        Framebuffer hdrFramebuffer;
        std::shared_ptr<Texture2D> hdrColorTexture = nullptr;
        std::shared_ptr<Renderbuffer> hdrDepthRenderbuffer = nullptr;
        std::shared_ptr<Buffer<QuadVertex>> hdrQuadBuffer = std::make_shared<Buffer<QuadVertex>>(BufferUsage::STATIC_DRAW);
        VertexArray hdrQuadVAO{};

        std::vector<MeshRecord> meshRecords;
        std::vector<Texture2D> materialTextures;
        std::vector<Material> materials;
        std::shared_ptr<Buffer<Vertex>> vertexBuffer = std::make_shared<Buffer<Vertex>>(BufferUsage::STATIC_DRAW);
        std::shared_ptr<Buffer<unsigned int>> indexBuffer = std::make_shared<Buffer<unsigned int>>(
                BufferUsage::STATIC_DRAW);
    };
} // graphics