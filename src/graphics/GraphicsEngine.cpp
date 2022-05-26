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

    vertexBuffer.reset(new VertexBuffer());
    indexBuffer.reset(new IndexBuffer());
}

void graphics::GraphicsEngine::update() {
    loadQueueIntoBuffers();

    if (vertexBuffer->isDirty()) vertexBuffer->upload();
    if (indexBuffer->isDirty()) indexBuffer->upload();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    camera.use(shader);

    // For all models, add any new mesh mats.

    runDrawCommands();

    window.swapBuffers();
}


void graphics::GraphicsEngine::runDrawCommands() {
    if (drawCommandSet.empty()) return;

    auto currentMesh = drawCommandSet.begin()->object->getMesh();
    std::vector<glm::mat4> upcomingDrawMats;
    std::vector<glm::vec4> upcomingDrawColors;

    auto drawMesh = [&upcomingDrawColors, &upcomingDrawMats, this](MeshPtr mesh) {
        auto &record = this->loadedMeshes.at(mesh);

        uint instanceCount = upcomingDrawColors.size();

        record.matBuffer->setModelMats(upcomingDrawMats);
        record.colorBuffer->setVertices(upcomingDrawColors);

        record.matBuffer->upload();
        record.colorBuffer->upload();

        record.arrayObject->bind();

        glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndices().size(), GL_UNSIGNED_INT, (const GLvoid *)(record.indicesIndex * sizeof(uint)), instanceCount);

        upcomingDrawMats.clear();
        upcomingDrawColors.clear();
    };

    for (auto &command: drawCommandSet) {
        auto &object = command.object;
        // If the end of the current mesh bucket, render the bucket and reset
        if (object->getMesh() != currentMesh) {
            drawMesh(currentMesh);



            currentMesh = command.object->getMesh();
        }

        upcomingDrawColors.push_back(object->getColor());
        upcomingDrawMats.push_back(object->getModelMatrix());
    }

    drawMesh(currentMesh);

    drawCommandSet.clear();
}

void graphics::GraphicsEngine::setCamera(graphics::Camera &newCamera) {
    camera = newCamera;
}

void graphics::GraphicsEngine::setViewportSize(std::pair<int, int> newSize) {
    glViewport(0, 0, newSize.first, newSize.second);
    viewportSize = newSize;
}

void graphics::GraphicsEngine::loadMesh(graphics::MeshPtr mesh) {
    // If mesh is already loaded, don't add it
    if (loadedMeshes.contains(mesh)) return;
    loadedMeshes.emplace(mesh, MeshRecord());
    meshLoadingQueue.push(mesh);
}

void graphics::GraphicsEngine::draw(ObjectPtr object) {
    loadMesh(object->getMesh());

    DrawCommand command;
    command.object = object;

    drawCommandSet.insert(command);
}

void graphics::GraphicsEngine::loadQueueIntoBuffers() {
    while (!meshLoadingQueue.empty()) {

        auto mesh = meshLoadingQueue.front();
        auto &meshRecord = loadedMeshes.at(mesh);

        meshRecord.verticesIndex = vertexBuffer->getSize();
        meshRecord.indicesIndex = indexBuffer->getSize();
        meshRecord.colorBuffer = std::make_shared<ColorVertexBuffer>();
        meshRecord.matBuffer = std::make_shared<ModelMatBuffer>();

        std::cout << "New mesh, loading at vertex index " << meshRecord.verticesIndex << ", indices index " << meshRecord.indicesIndex << std::endl;
        std::cout << "with vertices size of " << mesh->getVertices().size() << " indices size of " << mesh->getIndices().size() << std::endl;

        VertexArrayBuilder builder;
        builder.addBuffer(vertexBuffer);
        builder.addBuffer(indexBuffer);
        builder.addBuffer(meshRecord.colorBuffer);
        builder.addBuffer(meshRecord.matBuffer);

        meshRecord.arrayObject = builder.build();

        auto &meshIndices = mesh->getIndices();

        for (auto index : meshIndices){
            indexBuffer->addIndex(index + meshRecord.verticesIndex);
        }

        vertexBuffer->addVertices(mesh->getVertices());

        meshLoadingQueue.pop();
    }
}


bool graphics::GraphicsEngine::DrawCommand::operator<(const graphics::GraphicsEngine::DrawCommand &other) const {
    // Separate by mesh used
    return other.object->getMesh() < object->getMesh();
}
