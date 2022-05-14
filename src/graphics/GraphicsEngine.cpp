//
// Created by hiram on 4/23/22.
//

#include "GraphicsEngine.h"
#include "VertexArrayBuilder.h"


graphics::GraphicsEngine::GraphicsEngine(std::pair<int, int> viewSize) : camera(glm::dvec3(0.0, 0.0, 0.0), 35.0,
                                                                                viewSize, 0.0, 0.0),
                                                                         shader("/home/hiram/Projects/cityy/shaders/vertex.vsh",
                                                                                "/home/hiram/Projects/cityy/shaders/fragment.fsh") {}

void graphics::GraphicsEngine::start() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto viewSize = camera.getViewSize();
    window = glfwCreateWindow(viewSize.first, viewSize.second, "cityy", nullptr, nullptr);

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, viewSize.first, viewSize.second);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        auto engine = (GraphicsEngine *) glfwGetWindowUserPointer(window);
        engine->onViewSizeChange(window, width, height);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xPos, double yPos) {
        auto engine = (GraphicsEngine *) glfwGetWindowUserPointer(window);
        engine->onMouseMove(window, xPos, yPos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow *window, double xOffset, double yOffset) {
        auto engine = (GraphicsEngine *) glfwGetWindowUserPointer(window);
        engine->onMouseScroll(window, xOffset, yOffset);
    });

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glFrontFace(GL_CCW);
//    glCullFace(GL_FRONT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // TODO make shader follow RAII principle
    shader.init();

    VBO = std::make_shared<VertexBuffer>();
    EBO = std::make_shared<IndexBuffer>();
}

void graphics::GraphicsEngine::onViewSizeChange(GLFWwindow *window, int width, int height) {
    camera.setViewSize(std::make_pair(width, height));
    glViewport(0, 0, width, height);
}

void graphics::GraphicsEngine::onMouseScroll(GLFWwindow *window, double xOffset, double yOffset) {
    static const float minFov = 1.0f;
    static const float maxFov = 35.0f;

    camera.setFov(camera.getFov() - yOffset);

    const double fov = camera.getFov();

    if (fov > maxFov) {
        camera.setFov(maxFov);
    } else if (fov < minFov) {
        camera.setFov(minFov);
    }
}

void graphics::GraphicsEngine::onMouseMove(GLFWwindow *window, double xPos, double yPos) {
    float xOffset = (float) xPos - lastMousePos.first;
    float yOffset = lastMousePos.second - (float) yPos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        camera.setYaw(camera.getYaw() + xOffset);
        camera.setPitch(camera.getPitch() + yOffset);
    }

    lastMousePos = std::make_pair(xPos, yPos);
}

void graphics::GraphicsEngine::handleInput() {
    const float cameraSpeed = 2.5f * deltaTime;
    auto cameraPos = camera.getCameraPos();
    auto cameraFront = camera.getCameraFront();
    auto cameraUp = camera.getCameraUp();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cameraPos += glm::normalize(-cameraUp) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cameraPos += glm::normalize(cameraUp) * cameraSpeed;
    }
    camera.setCameraPos(cameraPos);


}

void graphics::GraphicsEngine::update() {
    // For every item in the loading queue:
    while (!objectLoadingQueue.empty()) {
        // Object
        auto object = objectLoadingQueue.front();
        //Get the instances model data
        auto model = object->getModel();
        auto modelName = model->getName();

        // If model is not loaded, add it to add it to newVertices
        if (modelMap.count(modelName) == 0) {
            // Get vertices
            auto modelVertices = model->getVertices();
            // Get the location in the VBO of the model
            auto start = VBO->getSize();
            auto location = ModelLoc(start, modelVertices.size());

            // Initialize data of this model
            ModelData data;
            data.location = location;
            data.modelMats = std::make_shared<ModelMatBuffer>();

            VertexArrayBuilder builder;
            builder.addBuffer(VBO);
            builder.addBuffer(data.modelMats);

            // If the model has index data, add it to the total data and bind EBO.
            if (model->usesElements()) {
                auto modelIndices = model->getIndices();
                auto indicesStart = EBO->getSize();
                auto indicesLocation = ModelLoc(indicesStart, modelIndices.size());

                data.indicesLocation = {indicesLocation};
                builder.addBuffer(EBO);

                std::for_each(modelIndices.begin(), modelIndices.end(), [&](uint index) {
                    EBO->addIndex(index + location.first);
                });
            }

            data.modelVAO = builder.build();
            modelMap[modelName] = data;
            VBO->addVertices(modelVertices);
        }

        // Add object to the vector
        auto &objectVector = modelMap[modelName].instances;
        auto modelMatrix = object->getModelMatrix();
        objectVector.insert(objectVector.end(), object);

        modelMap[modelName].modelMats->addModelMat(modelMatrix);
        // Remove object from the newModelMats
        objectLoadingQueue.pop();
    }

    if (VBO->isDirty()) {
        // add and load everything at once.
        VBO->upload();
    }
    if (EBO->isDirty()) {
        EBO->upload();
    }

    deltaTime = (float) glfwGetTime() - lastUpdateTime;
    lastUpdateTime = (float) glfwGetTime();
    handleInput();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    shader.use();
    camera.use(shader);

    // For all models, add any new model mats.
    for (auto &modelType: modelMap) {
        auto &data = modelType.second;
        // If the model mats haven't been added, add them.
        if (data.modelMats->isDirty()) {
            data.modelMats->upload();
        }

        // Bind model data
        data.modelVAO->bind();
        auto &location = data.location;

        // If model has indices, render as elements.
        // Else render vertices directly
        if (data.indicesLocation.has_value()) {
            auto &indicesLocation = data.indicesLocation.value();
            glDrawElementsInstanced(GL_TRIANGLES, indicesLocation.second, GL_UNSIGNED_INT,
                                    (const GLvoid *) (indicesLocation.first * sizeof(uint)),
                                    modelType.second.instances.size());
        } else {
            glDrawArraysInstanced(GL_TRIANGLES, location.first, location.second, data.instances.size());
        }
    }

    glfwPollEvents();
    glfwSwapBuffers(window);
}

bool graphics::GraphicsEngine::shouldRun() {
    return !glfwWindowShouldClose(window);
}

graphics::GraphicsEngine::~GraphicsEngine() {
    glfwTerminate();
}

void graphics::GraphicsEngine::addObject(const graphics::GraphicsEngine::ObjectPtr &object) {
    objectLoadingQueue.push(object);
}






