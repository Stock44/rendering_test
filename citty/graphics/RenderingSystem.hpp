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

namespace citty::graphics {
//    class GLADInitError : public std::runtime_error {
//    public:
//        GLADInitError() : std::runtime_error("Failed to initialize GLAD") {};
//    };
//
//    struct RenderCommand {
//        long meshID;
//        long bufferPosition;
//
//        bool bucketWith(RenderCommand const &other) const {
//            return meshID == other.meshID;
//        }
//
//        bool operator==(RenderCommand const &rhs) const = default;
//
//        std::weak_ordering operator<=>(RenderCommand const &rhs) const {
//            // Lexicographical comparison, first meshID then bufferPosition
//            return std::tie(meshID, bufferPosition) <=> std::tie(rhs.meshID, rhs.bufferPosition);
//        }
//    };
//
//    template<typename T>
//    struct DerefLess {
//        bool operator()(std::unique_ptr<T> const &lhs, std::unique_ptr<T> const &rhs) const {
//            return *lhs < *rhs;
//        }
//    };
//
//    struct MeshRecord {
//        ModelMatBuffer matBuffer;
//        ColorVertexBuffer colorBuffer;
//        VertexArray arrayObject; // VAO of this specific mesh (should be bound to engine VBO and modelMats of this mesh
//        long verticesIndex;
//        long indicesIndex;
//        long meshSize;
//        bool dirty;
//    };
//
    struct MeshRecord {
        std::shared_ptr<Buffer<Eigen::Matrix4f>> matBuffer;
        VertexArray vertexArrayObject;
        std::size_t verticesOffset;
        std::size_t indicesOffset;
    };

    class RenderingSystem : public engine::System {
    public:
        explicit RenderingSystem(Gtk::GLArea *glArea);

        void init() override;

        void update() override;

        void render();

    private:
        using Id = int_fast64_t;
        const Id MATERIAL_ID_MAX = INT32_MAX;
        const int MATERIAL_ID_SIZE = 32;
        const Id MESH_ID_MAX = INT32_MAX;
        const int MESH_ID_SIZE = 32;

        void handleTextures();

        void loadTextures();

        void handleMeshes();

        void loadMeshes();

        std::unordered_set<engine::Entity> loadedTextureEntities;
        std::queue<std::pair<engine::Entity, Texture>> textureLoadQueue;
        std::unordered_map<engine::Entity, TextureObject> textureObjects;
        std::mutex textureLock;

        std::unordered_set<engine::Entity> loadedMeshEntities;
        std::queue<std::pair<engine::Entity, Mesh>> meshLoadingQueue;
        std::unordered_map<Id, MeshRecord> meshRecords;
        std::unordered_map<engine::Entity, Id> meshIds; // maps ecs Mesh entity to a specific mesh id
        Id nextMeshId = 0;
        std::mutex meshLock;

        // shared buffers for non-mutable meshes
        std::shared_ptr<Buffer<Vertex>> vertexBuffer = nullptr;
        std::shared_ptr<Buffer<unsigned int>> indexBuffer = nullptr;

//        RenderingSystem(Gtk::GLArea *gl_area);
//
//        void render();
//
//        void init() override;
//
//        void update() override;
//
//        void stageMeshIntoBuffers(Mesh const &mesh);
//
//        void regenerateBuckets();
//
//        void tryRegisterEntity(Entity entity);
//
//        void onMeshCreate(EntitySet entities);
//
//        void onTransformCreate(EntitySet entities);
//
//        void onColorCreate(EntitySet entities);
//
//        void onTransformUpdate(EntitySet entities);
//
//        void onColorUpdate(EntitySet entities);
//
//        void onCameraCreate(EntitySet entities);
//
//        void setViewportSize(int width, int height);
//
//        void updateViewMatrix(Transform const &transform);
//
//        void updateProjectionMatrix(Camera camera);
//
//    private:
//        std::unique_ptr<Shader> shader;
//
//        std::optional<Entity> cameraEntity;
//
//        std::pair<int, int> viewportSize;
//
//        std::unique_ptr<VertexBuffer> vertexBuffer;
//        std::unique_ptr<IndexBuffer> indexBuffer;
//
//        std::vector<std::unique_ptr<RenderCommand>> renderCommands;
//        std::unordered_map<Entity, RenderCommand *> entityRenderMap;
//        std::unordered_map<long, MeshRecord> loadedMeshes;
//
//        // Bucket ranges, start position and size
//        std::vector<std::pair<int, int>> buckets;
    };

} // graphics
