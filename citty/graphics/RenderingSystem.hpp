//
// Created by hiram on 6/6/22.
//

#pragma once

#include <gtkmm/glarea.h>
#include <citty/engine/System.hpp>
#include <citty/graphics/components/Mesh.hpp>
#include <citty/graphics/components/Vertex.hpp>
#include <citty/graphics/components/Camera.hpp>
#include <citty/graphics/components/Texture.hpp>
#include <citty/graphics/components/Graphics.hpp>
#include <citty/graphics/Buffer.hpp>
#include <citty/graphics/VertexArray.hpp>
#include <citty/graphics/Shader.hpp>
#include <citty/graphics/ShaderProgram.hpp>
#include <mutex>
#include <citty/graphics/components/Material.hpp>
#include <cmath>
#include <numbers>

namespace citty::graphics {
    struct MeshRecord {
        std::shared_ptr<Buffer<Eigen::Affine3f>> transformBuffer;
        std::vector<Eigen::Affine3f> transformData;
        VertexArray vertexArrayObject;
        std::size_t verticesOffset;
        std::size_t indicesOffset;
        std::size_t indicesSize;
    };

    Eigen::Projective3f perspective(float verticalFoV, float aspectRatio, float zNear, float zFar);

    Eigen::Affine3f
    lookAt(Eigen::Vector3f const &cameraPos, Eigen::Vector3f const &targetPosition, Eigen::Vector3f const &up);


    class RenderingSystem : public engine::System {
    public:
        explicit RenderingSystem(Gtk::GLArea *glArea);

        void init() override;

        void update() override;

        void render();

    private:
        // TODO implement checks for reaching the maximum number of meshes or materials (improbable, but good practice)
        using Id = int_fast64_t;

        const int MESH_ID_OFFSET = 0;
        const int MESH_ID_SIZE = 32;
        const Id MESH_ID_MAX = INT32_MAX;

        const int MATERIAL_ID_OFFSET = MESH_ID_SIZE;
        const int MATERIAL_ID_SIZE = 32;
        const Id MATERIAL_ID_MAX = INT32_MAX;

        void handleTextures();

        void loadTextures();

        void handleMaterials();

        void handleMeshes();

        void loadMeshes();

        void handleGraphicsEntities();

        void renderGraphicsEntities();

        void loadEntityTransforms();

        void enableMaterial(Id materialId);

        void drawMesh(Id meshId);

        ShaderProgram shaderProgram{0};

        std::unordered_set<engine::Entity> loadedTextureEntities;
        std::queue<std::pair<engine::Entity, Texture>> textureLoadQueue;
        std::unordered_map<engine::Entity, TextureObject> textureObjects;
        std::mutex textureLock;

        std::queue<std::pair<engine::Entity, Mesh>> meshLoadingQueue;
        std::unordered_map<Id, MeshRecord> meshRecords;
        std::unordered_map<engine::Entity, Id> meshIds; // maps ecs Mesh entity to a specific mesh id
        Id nextMeshId = 0;
        std::mutex meshLock;

        std::unordered_map<Id, Material> materials;
        std::unordered_map<engine::Entity, Id> materialIds;
        Id nextMaterialId = 0;
        std::mutex materialLock;

        // shared buffers for non-mutable meshes
        std::shared_ptr<Buffer<Vertex>> vertexBuffer = nullptr;
        std::shared_ptr<Buffer<unsigned int>> indexBuffer = nullptr;

        std::vector<Id> drawIds;
        std::mutex transformsLock;

        std::pair<int, int> viewportDimensions;
    };

} // graphics
