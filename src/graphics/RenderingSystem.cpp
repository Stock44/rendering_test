//
// Created by hiram on 6/6/22.
//

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <algorithm>
#include <iterator>
#include "RenderingSystem.h"
#include "components/Camera.h"

namespace graphics {
    RenderingSystem::RenderingSystem(Window &window) : window(window) {
        window.useWindowContext();

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw GLADInitError();
        }

        indexBuffer = std::make_unique<IndexBuffer>();
        vertexBuffer = std::make_unique<VertexBuffer>();

        shader = std::make_unique<Shader>("/home/hiram/Projects/citty/shaders/vertex.vsh",
                                          "/home/hiram/Projects/citty/shaders/fragment.fsh");

        setViewportSize(window.getSize());

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_FRONT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    }

    void RenderingSystem::setup(engine::ComponentManager &componentManager) {
        meshStore = componentManager.getComponentStore<MeshRef>();
        colorStore = componentManager.getComponentStore<Color>();
        transformStore = componentManager.getComponentStore<Transform>();
        cameraStore = componentManager.getComponentStore<Camera>();

        meshStore->onComponentCreation([this](EntitySet entities) { onMeshCreate(entities); });
        colorStore->onComponentCreation([this](EntitySet entities) { onColorCreate(entities); });
        colorStore->onComponentUpdate([this](EntitySet entities) { onColorUpdate(entities); });
        transformStore->onComponentCreation([this](EntitySet entities) { onTransformCreate(entities); });
        transformStore->onComponentUpdate([this](EntitySet entities) { onTransformUpdate(entities); });
        cameraStore->onComponentCreation([this](EntitySet entities) { onCameraCreate(entities); });

        window.setViewSizeCallback([this](auto newSize) {
            setViewportSize(newSize);
            if (cameraEntity.has_value()) {
                updateProjectionMatrix(cameraStore->getComponent(cameraEntity.value()));
            }
        });
    }

    void RenderingSystem::update(engine::EntityManager &elementManager) {
        for (auto &[id, meshRecord]: loadedMeshes) {
            // Upload mesh records if they are dirty.
            // Also regenerate buckets
            // TODO better implementation of bucket regeneration
            if (meshRecord.dirty) {
                meshRecord.colorBuffer.upload();
                meshRecord.matBuffer.upload();
                meshRecord.dirty = false;
                regenerateBuckets();
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Only try to render if there is a camera
        if (cameraEntity.has_value()) {
            shader->use();

            // For each bucket, render its contents
            for (auto const &[bucketIndex, bucketSize]: buckets) {
                // Leading command for this bucket
                auto command = *renderCommands.at(bucketIndex);
                auto const &meshRecord = loadedMeshes.at(command.meshID);
                meshRecord.arrayObject.bind();
                glDrawElementsInstanced(GL_TRIANGLES, meshRecord.meshSize, GL_UNSIGNED_INT,
                                        (const GLvoid *) (meshRecord.indicesIndex * sizeof(uint)),
                                        bucketSize);
            }
        }

        window.swapBuffers();
    }

    void RenderingSystem::regenerateBuckets() {
        buckets.clear();
        auto currentCommand = **renderCommands.begin();
        long bucketSize = 0;
        long currentIndex = 0;

        // Sort the render commands before regenerating the buckets
        std::sort(renderCommands.begin(), renderCommands.end(), DerefLess<RenderCommand>());

        for (auto const &command: renderCommands) {
            // Lexicographical comparison
            bool inBucket = currentCommand.bucketWith(*command);

            if (inBucket) {
                bucketSize++;
            }
            if (!inBucket || currentIndex >= renderCommands.size() - 1) {

                buckets.emplace_back(currentIndex, bucketSize);

                currentCommand = *command;
                bucketSize = 0;
            }
            currentIndex++;
        }
    }

    void RenderingSystem::stageMeshIntoBuffers(Mesh const &mesh) {
        MeshRecord meshRecord;
        meshRecord.arrayObject.bind();
        vertexBuffer->enableAttribs();
        indexBuffer->enableAttribs();
        meshRecord.colorBuffer.enableAttribs();
        meshRecord.matBuffer.enableAttribs();

        meshRecord.verticesIndex = vertexBuffer->getSize();
        meshRecord.indicesIndex = indexBuffer->getSize();
        meshRecord.meshSize = std::ssize(mesh.indices);

        // Shift indices to their actual positions on the buffer
        auto const &indices = mesh.indices;

        std::vector<GLuint> adjustedIndices;

        std::ranges::transform(indices, std::back_inserter(adjustedIndices), [&meshRecord](GLuint index) {
            return index + meshRecord.verticesIndex;
        });

        // Stage the vertices and adjusted indices into their buffers
        vertexBuffer->addVertices(mesh.vertices);
        indexBuffer->addIndices(adjustedIndices);

        vertexBuffer->upload();
        indexBuffer->upload();

        // Store the loaded mesh record into the map
        loadedMeshes.try_emplace(mesh.ID, std::move(meshRecord));
    }

    // Try registering the entity into the rendering system, depending on whether the entity
    // has a mesh, a color and a transform component.
    void RenderingSystem::tryRegisterEntity(Entity entity) {
        // TODO find out if this check is necessary
        // If this entity is already registered, skip it.
        if (entityRenderMap.contains(entity)) return;
        // If this entity doesn't have a mesh don't do anything
        if (!meshStore->hasComponent(entity)) return;

        Mesh const &mesh = meshStore->getComponent(entity);
        // If this mesh isn't loaded yet, load it
        if (!loadedMeshes.contains(mesh.ID)) {
            stageMeshIntoBuffers(mesh);
        }

        // Don't queue the render if the entity doesn't have a transform or color
        if (!transformStore->hasComponent(entity) || !colorStore->hasComponent(entity)) return;

        auto color = colorStore->getComponent(entity);
        auto transform = transformStore->getComponent(entity);

        // Calculate model matrix for this entity
        auto modelMatrix = transform.calculateMatrix();

        // Load the transform and colors into the matrix and color buffers
        auto &meshRecord = loadedMeshes.at(mesh.ID);
        auto &matBuffer = meshRecord.matBuffer;
        auto &colorBuffer = meshRecord.colorBuffer;

        // Create render command and queue it
        auto command = std::make_unique<RenderCommand>();
        command->meshID = mesh.ID;
        command->bufferPosition = matBuffer.getSize();
        entityRenderMap.try_emplace(entity, command.get());
        renderCommands.push_back(std::move(command));

        // Add matrix and color to buffers
        matBuffer.addModelMat(modelMatrix);
        colorBuffer.addVertex(color);
        meshRecord.dirty = true;
    }

    // TODO implement onMeshUpdate
    void RenderingSystem::onMeshCreate(EntitySet entities) {
        for (auto entity: entities) {
            tryRegisterEntity(entity);
        }
    }

    void RenderingSystem::onColorCreate(EntitySet entities) {
        for (auto entity: entities) {
            tryRegisterEntity(entity);
        }
    }


    void RenderingSystem::onTransformCreate(EntitySet entities) {
        for (auto entity: entities) {
            if (cameraStore->hasComponent(entity) && cameraEntity.has_value() && entity == cameraEntity.value())
                updateViewMatrix(transformStore->getComponent(entity));

            tryRegisterEntity(entity);
        }
    }

    void RenderingSystem::onTransformUpdate(EntitySet entities) {
        for (auto entity: entities) {
            // If this transform is associated with a camera, update the view matrix
            if (cameraStore->hasComponent(entity) && cameraEntity.has_value() && entity == cameraEntity.value())
                updateViewMatrix(transformStore->getComponent(entity));

            // If this entity is not rendered, ignore
            if (!entityRenderMap.contains(entity)) continue;

            auto meshID = entityRenderMap.at(entity)->meshID;
            auto bufferPosition = entityRenderMap.at(entity)->bufferPosition;

            MeshRecord &meshRecord = loadedMeshes.at(meshID);

            auto newModelMatrix = transformStore->getComponent(entity).calculateMatrix();

            meshRecord.matBuffer.setModelMat(newModelMatrix, bufferPosition);

            meshRecord.dirty = true;
        }
    }

    void RenderingSystem::onColorUpdate(EntitySet entities) {
        for (auto entity: entities) {
            // If this entity is not rendered, ignore
            if (!entityRenderMap.contains(entity)) continue;

            auto meshID = entityRenderMap.at(entity)->meshID;
            auto bufferPosition = entityRenderMap.at(entity)->bufferPosition;

            MeshRecord &meshRecord = loadedMeshes.at(meshID);

            auto newColor = colorStore->getComponent(entity);

            meshRecord.colorBuffer.setVertex(newColor, bufferPosition);

            meshRecord.dirty = true;
        }
    }

    void RenderingSystem::onCameraCreate(EntitySet entities) {
        cameraEntity = *entities.begin();

        Transform cameraTransform{};
        // If this camera entity has a transform, use it.
        // Else just use a default transform
        if (transformStore->hasComponent(cameraEntity.value()))
            cameraTransform = transformStore->getComponent(cameraEntity.value());

        auto cameraComponent = cameraStore->getComponent(cameraEntity.value());

        // Update matrices
        updateViewMatrix(cameraTransform);
        updateProjectionMatrix(cameraComponent);
    }

    void RenderingSystem::setViewportSize(const std::pair<int, int> &newViewportSize) {
        RenderingSystem::viewportSize = newViewportSize;
        glViewport(0, 0, newViewportSize.first, newViewportSize.second);
    }

    void RenderingSystem::updateViewMatrix(Transform const &transform) {
        // Rotate front and up vectors according to camera's transform
        auto cameraDirection = transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
        auto cameraUp = transform.rotation * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::mat4 view = glm::lookAt(transform.position, transform.position + cameraDirection, cameraUp);
        shader->use();
        shader->setMatrix("view", view);
    }

    void RenderingSystem::updateProjectionMatrix(Camera camera) {
        glm::mat4 proj = glm::perspective(glm::radians(camera.fov),
                                          static_cast<float>(viewportSize.first) /
                                          static_cast<float>(viewportSize.second), 10.0f, 100000.0f);
        shader->use();
        shader->setMatrix("projection", proj);
    }

} // graphics