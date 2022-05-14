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
#include <queue>
#include <unordered_map>

namespace graphics {
    // TODO extract window startup and input handling out of here
    class GraphicsEngine {
    public:
        typedef std::shared_ptr<Object> ObjectPtr;
        typedef std::pair<uint, uint> ModelLoc;
        // TODO implement removal of objects from rendering:
        // use sequential IDs per model type
        typedef std::pair<ObjectPtr, int> ObjectTracker; // Keep object Id in tracker pair object,
        // TODO check if this is necessary

        struct ModelData {
            ModelLoc location; // Model location in engine's VBO
            std::optional<ModelLoc> indicesLocation; // If model uses elements, location of the indices in the EBO.
            std::vector<ObjectPtr> instances; // Model instances
            std::shared_ptr<VertexArray> modelVAO; // VAO of this specific model (should be bound to engine VBO and modelMats of this model
            std::shared_ptr<ModelMatBuffer> modelMats; // MBO of this model's instances
        };

        ~GraphicsEngine();

        explicit GraphicsEngine(std::pair<int, int> viewSize);

        void start();

        void onViewSizeChange(GLFWwindow *window, int width, int height);

        void onMouseScroll(GLFWwindow *window, double xOffset, double yOffset);

        void onMouseMove(GLFWwindow *window, double xPos, double yPos);

        void handleInput();

        bool shouldRun();

        void update();

        void addObject(const ObjectPtr& object);

    private:
        GLFWwindow *window;
        float deltaTime = 0.0f;
        float lastUpdateTime = 0.0f;
        Camera camera;
        Shader shader;
        std::pair<float, float> lastMousePos;
        float mouseSensitivity = 0.2f;

        std::shared_ptr<VertexBuffer> VBO;
        std::shared_ptr<IndexBuffer> EBO;

        std::queue<ObjectPtr> objectLoadingQueue;
        std::unordered_map<std::string, ModelData> modelMap; // Find model data by the model name


    };
}


#endif //CITYY_GRAPHICSENGINE_H
