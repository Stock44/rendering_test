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

namespace graphics {
    // TODO implement color for objects
    // TODO implement textures for objects
    class GraphicsEngine {
    public:
        typedef std::pair<std::string, int> ObjectID;
        typedef std::shared_ptr<Object> ObjectPtr;
        typedef std::shared_ptr<Model> ModelPtr;
        typedef std::pair<uint, uint> ModelLoc;

        struct ModelData {
            ModelLoc location; // Model location in engine's VBO
            std::optional<ModelLoc> indicesLocation; // If model uses elements, location of the indices in the EBO.
            std::vector<ObjectPtr> instances; // Model instances
            std::shared_ptr<VertexArray> modelVAO = nullptr; // VAO of this specific model (should be bound to engine VBO and modelMats of this model
            std::shared_ptr<ColorVertexBuffer> colors = nullptr; // Vertex buffer to store this model's instance colors.
            std::shared_ptr<ModelMatBuffer> modelMats = nullptr; // MBO of this model's instances
        };

        explicit GraphicsEngine(Window &window, Camera &camera);

        void update();

        // TODO function to delete a model from memory
        void addModel(ModelPtr model);

        ObjectID addObject(const ObjectPtr &object);

        void deleteObject(const ObjectID &id);

        void setCamera(Camera &newCamera);

        void setViewportSize(std::pair<int, int> newSize);

    private:
        Window &window;
        Camera &camera;
        Shader shader;

        std::pair<int, int> viewportSize;

        std::shared_ptr<VertexBuffer> VBO = nullptr;
        std::shared_ptr<IndexBuffer> EBO = nullptr;

        std::queue<ObjectPtr> objectLoadingQueue;
        std::unordered_map<std::string, ModelData> modelMap; // Find model data by the model name


    };
}


#endif //CITYY_GRAPHICSENGINE_H
