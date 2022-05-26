//
// Created by hiram on 4/23/22.
//

#ifndef CITYY_GRAPHICSENGINE_H
#define CITYY_GRAPHICSENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Object.h"
#include "Camera.h"
#include "buffers/VertexBuffer.h"
#include "buffers/ModelMatBuffer.h"
#include "VertexArray.h"
#include "buffers/IndexBuffer.h"
#include "../Window.h"
#include "buffers/ColorVertexBuffer.h"
#include <queue>
#include <unordered_map>
#include <set>

namespace graphics {
    // TODO implement color for objects
    // TODO implement textures for objects
    class GraphicsEngine {
    public:
        struct DrawCommand {
            ObjectPtr object;

            // Provides ordering of commands into buckets by multiset
            bool operator<(const DrawCommand &other) const;
        };

        struct MeshRecord {
            uint verticesIndex;
            uint indicesIndex;
            std::shared_ptr<VertexArray> arrayObject = nullptr; // VAO of this specific mesh (should be bound to engine VBO and modelMats of this mesh
            std::shared_ptr<ColorVertexBuffer> colorBuffer = nullptr;
            std::shared_ptr<ModelMatBuffer> matBuffer = nullptr;
        };

        explicit GraphicsEngine(Window &window, Camera &camera);

        void loadMesh(MeshPtr mesh);

        void draw(ObjectPtr object);

        void update();

        void setCamera(Camera &newCamera);

        void setViewportSize(std::pair<int, int> newSize);

    private:

        void loadQueueIntoBuffers();

        void runDrawCommands();

        Window &window;
        Camera &camera;
        Shader shader;

        std::pair<int, int> viewportSize;

        std::shared_ptr<VertexBuffer> vertexBuffer = nullptr; // Shared VBO for all models
        std::shared_ptr<IndexBuffer> indexBuffer = nullptr; // Shared EBO for all models

        std::unordered_map<MeshPtr, MeshRecord> loadedMeshes; // Meshes that have been loaded and their positions on the VBO and IBO;
        std::queue<MeshPtr> meshLoadingQueue;
        std::multiset<DrawCommand> drawCommandSet; // Provides bucketing of commands based on DrawCommand < less operator

        std::mutex objectMutex, modelMutex;
    };
}


#endif //CITYY_GRAPHICSENGINE_H
