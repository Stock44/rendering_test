//
// Created by hiram on 4/23/22.
//

#include "GraphicsEngine.h"
#include "VertexArrayBuilder.h"


graphics::GraphicsEngine::GraphicsEngine(Window &window, Camera &camera) : window(window),
                                                                           camera(camera),
                                                                           viewportSize(window.getSize()),
                                                                           shader("/home/hiram/Projects/citty/shaders/vertex.vsh",
                                                                  "/home/hiram/Projects/citty/shaders/fragment.fsh") {
    window.useWindowContext();

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, viewportSize.first, viewportSize.second);

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glFrontFace(GL_CCW);
//    glCullFace(GL_FRONT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // TODO make shader follow RAII principle
    shader.init();

    VBO.reset(new VertexBuffer());
    EBO.reset(new IndexBuffer());
}

void graphics::GraphicsEngine::update() {
    if (VBO->isDirty()) {
        VBO->upload();
    }
    if (EBO->isDirty()) {
        EBO->upload();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    camera.use(shader);

    // For all models, add any new model mats.
    for (auto &modelType: modelMap) {
        auto &data = modelType.second;

        auto &instances = data.instances;

        for(auto it = instances.begin(); it != instances.end(); it++) {
            auto object = it->get();
            if(object->isDirty()){
                int index = std::distance(instances.begin(), it);
                data.modelMats->replaceModelMat(object->getModelMatrix(), index);
                object->setDirty(false);
            }
        }

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

    window.swapBuffers();
}

graphics::GraphicsEngine::ObjectID graphics::GraphicsEngine::addObject(const graphics::GraphicsEngine::ObjectPtr &object) {
    auto model = object->getModel();
    auto modelName = model->getName();

    // If model is not loaded, add it to add it to newVertices
    if (modelMap.count(modelName) == 0) {
       addModel(model);
    }

    // Add object to the vector
    auto &objectVector = modelMap[modelName].instances;
    auto modelMatrix = object->getModelMatrix();
    objectVector.insert(objectVector.end(), object);

    modelMap[modelName].modelMats->addModelMat(modelMatrix);
    // Remove object from the newModelMats
    return {object->getModel()->getName(), modelMap.at(modelName).instances.size() - 1 };
}

void graphics::GraphicsEngine::setCamera(graphics::Camera &newCamera) {
    camera = newCamera;
}

void graphics::GraphicsEngine::setViewportSize(std::pair<int, int> newSize) {
    glViewport(0, 0, newSize.first, newSize.second);
    viewportSize = newSize;
}

void graphics::GraphicsEngine::deleteObject(const ObjectID &id) {
    auto objectModelName = id.first;
    auto objectIndex = id.second;

    auto &modelData = modelMap.at(objectModelName);
    auto &instances = modelData.instances;
    auto modelMats = modelData.modelMats;

    instances.erase(instances.begin() + objectIndex);
    modelMats->deleteModelMat(objectIndex);
}

void graphics::GraphicsEngine::addModel(const graphics::GraphicsEngine::ModelPtr model) {
    // Get vertices
    auto modelVertices = model->getVertices();
    // Get the verticesLocation in the VBO of the model
    auto start = VBO->getSize();
    auto verticesLocation = ModelLoc(start, modelVertices.size());

    // Initialize newModelData of this model
    ModelData newModelData;
    newModelData.location = verticesLocation;
    newModelData.modelMats.reset(new ModelMatBuffer());

    VertexArrayBuilder builder;
    builder.addBuffer(VBO);
    builder.addBuffer(newModelData.modelMats);

    // If the model has index newModelData, add it to the total newModelData and bind EBO.
    if (model->usesElements()) {
        auto modelIndices = model->getIndices();
        auto indicesStart = EBO->getSize();
        auto indicesLocation = ModelLoc(indicesStart, modelIndices.size());

        newModelData.indicesLocation = {indicesLocation};
        builder.addBuffer(EBO);

        std::for_each(modelIndices.begin(), modelIndices.end(), [&](uint index) {
            EBO->addIndex(index + verticesLocation.first);
        });
    }

    newModelData.modelVAO = builder.build();
    modelMap[model->getName()] = newModelData;
    VBO->addVertices(modelVertices);
}








