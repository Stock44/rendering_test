//
// Created by hiram on 6/6/22.
//

#include <iterator>
#include <Eigen/Geometry>
#include <citty/graphics/RenderingSystem.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/ShaderProgramBuilder.hpp>


namespace citty::graphics {
    RenderingSystem::RenderingSystem(Gtk::GLArea *glArea) {
        glArea->signal_realize().connect([this, glArea]() {
            glArea->make_current();

            ShaderProgramBuilder builder;
            builder.addShader({"shaders/vertex.vsh", ShaderType::VERTEX});
            builder.addShader({"shaders/fragment.fsh", ShaderType::FRAGMENT});

            shaderProgram = builder.build();

            vertexBuffer = std::make_shared<Buffer<Vertex >>
                    (BufferUsage::STATIC_DRAW);
            indexBuffer = std::make_shared<Buffer<unsigned int >>
                    (BufferUsage::STATIC_DRAW);
            transformsBuffer = std::make_shared<Buffer<Eigen::Affine3f>>
                    (BufferUsage::STREAM_DRAW);

            glClearColor(0.4, 0.4, 0.8, 1);
        });

        glArea->signal_render().connect([this, glArea](Glib::RefPtr<Gdk::GLContext> const &glContext) {
            glClear(GL_COLOR_BUFFER_BIT);
            render();
            glArea->queue_draw();
            return true;
        }, false);
    }

    void RenderingSystem::init() {

    }

    void RenderingSystem::update() {
        handleTextures();
        handleMaterials();
        handleMeshes();
        handleGraphicsEntities();
    }

    void RenderingSystem::render() {
        std::cout << "rendering" << std::endl;
        loadTextures();
        loadMeshes();
        loadEntityTransforms();
        renderGraphicsEntities();
    }

    void RenderingSystem::handleTextures() {
        auto entities = getEntities<Texture>();
        auto [textures] = getComponents<Texture>();

        auto entityIt = entities.begin();
        auto textureIt = textures.begin();
        std::queue<std::pair<engine::Entity, Texture>> queue;
        while (entityIt != entities.end() && textureIt != textures.end()) {
            auto entity = *entityIt;
            auto &texture = *textureIt;

            if (!loadedTextureEntities.contains(entity)) {
                std::cout << "queueing texture for loading" << std::endl;
                loadedTextureEntities.emplace(entity);
                queue.emplace(*entityIt, texture);
            }

            entityIt++;
            textureIt++;
        }

        if (!queue.empty()) {
            std::scoped_lock lock{textureLock};
            textureLoadQueue = std::move(queue);
        }
    }

    void RenderingSystem::loadTextures() {
        if (textureLoadQueue.empty()) return;
        std::scoped_lock lock{textureLock};

        while (!textureLoadQueue.empty()) {
            std::cout << "loading texture" << std::endl;
            auto &[entity, texture] = textureLoadQueue.front();

            Image textureImage(texture.texturePath);
            auto [it, inserted] = textureObjects.try_emplace(entity, textureImage);
            textureLoadQueue.pop();

            auto &textureObject = it->second;

            textureObject.setTextureMagFilter(texture.magFilter);
            textureObject.setTextureMinFilter(texture.minFilter);
            textureObject.setTextureSWrapMode(texture.sWrappingMode);
            textureObject.setTextureTWrapMode(texture.tWrappingMode);
        }
    }

    void RenderingSystem::handleMaterials() {
        auto entities = getEntities<Material>();
        auto [entityMaterials] = getComponents<Material>();

        auto entityIt = entities.begin();
        auto entitiesEnd = entities.end();
        auto materialIt = entityMaterials.begin();
        auto materialEnd = entityMaterials.end();

        std::scoped_lock lock{materialLock};
        while (entityIt != entitiesEnd && materialIt != materialEnd) {
            auto entity = *entityIt;
            auto &material = *materialIt;

            if (!materialIds.contains(entity)) {
                std::cout << "loading new material" << std::endl;

                if (!loadedTextureEntities.contains(material.diffuseMap) ||
                    !loadedTextureEntities.contains(material.specularMap) ||
                    !loadedTextureEntities.contains(material.normalMap) ||
                    !loadedTextureEntities.contains(material.heightMap)) {
                    throw std::runtime_error("attempted to load material that contains unloaded textures");
                }

                materials.try_emplace(nextMaterialId, material);
                materialIds.try_emplace(entity, nextMaterialId);

                nextMaterialId++;
            }

            entityIt++;
            materialIt++;
        }
    }

    void RenderingSystem::handleMeshes() {
        auto entities = getEntities<Mesh>();
        auto [meshes] = getComponents<Mesh>();

        auto entityIt = entities.begin();
        auto meshesIt = meshes.begin();
        std::queue<std::pair<engine::Entity, Mesh>> queue;
        while (entityIt != entities.end() && meshesIt != meshes.end()) {
            auto entity = *entityIt;
            auto &mesh = *meshesIt;

            if (!meshIds.contains(entity)) {
                std::cout << "queued mesh for loading" << std::endl;
                meshIds.try_emplace(entity, nextMeshId);
                nextMeshId++;
                queue.emplace(*entityIt, mesh);
            }

            entityIt++;
            meshesIt++;
        }

        if (!queue.empty()) {
            std::scoped_lock lock{meshLock};
            meshLoadingQueue = std::move(queue);
        }
    }

    void RenderingSystem::loadMeshes() {
        if (meshLoadingQueue.empty()) return;
        std::scoped_lock lock{meshLock};

        while (!meshLoadingQueue.empty()) {
            std::cout << "loading mesh" << std::endl;
            auto &[entity, mesh] = meshLoadingQueue.front();

            MeshRecord meshRecord{
                    VertexArray{},
                    vertexBuffer->getSize(),
                    indexBuffer->getSize(),
                    mesh.indices.size(),
            };

            auto &vao = meshRecord.vertexArrayObject;

            vertexBuffer->append(mesh.vertices);
            indexBuffer->append(mesh.indices);

            vao.bindBuffer(vertexBuffer, meshRecord.verticesOffset);
            vao.enableAttrib(0);
            vao.configureAttrib(0, vertexBuffer, 3, AttributeType::FLOAT, false, 0);
            vao.enableAttrib(1);
            vao.configureAttrib(1, vertexBuffer, 3, AttributeType::FLOAT, false, offsetof(Vertex, normal));
            vao.enableAttrib(2);
            vao.configureAttrib(2, vertexBuffer, 3, AttributeType::FLOAT, false, offsetof(Vertex, tangent));
            vao.enableAttrib(3);
            vao.configureAttrib(3, vertexBuffer, 3, AttributeType::FLOAT, false, offsetof(Vertex, bitangent));
            vao.enableAttrib(4);
            vao.configureAttrib(4, vertexBuffer, 2, AttributeType::FLOAT, false, offsetof(Vertex, texCoords));

            vao.setVertexIndicesBuffer(indexBuffer);

            vao.bindBuffer(transformsBuffer, 0);
            vao.enableAttrib(5);
            vao.configureAttrib(5, transformsBuffer, 4, AttributeType::FLOAT, false, 0);
            vao.enableAttrib(5);
            vao.configureAttrib(6, transformsBuffer, 4, AttributeType::FLOAT, false, 4 * sizeof(float));
            vao.enableAttrib(5);
            vao.configureAttrib(7, transformsBuffer, 4, AttributeType::FLOAT, false, 8 * sizeof(float));
            vao.enableAttrib(5);
            vao.configureAttrib(8, transformsBuffer, 4, AttributeType::FLOAT, false, 12 * sizeof(float));
            vao.setBufferDivisor(transformsBuffer, 1);

            meshRecords.try_emplace(meshIds.at(entity), std::move(meshRecord));


            meshLoadingQueue.pop();
        }
    }

    void RenderingSystem::handleGraphicsEntities() {
        auto [transforms, graphics] = getComponents<engine::Transform, Graphics>();

        auto transformIt = transforms.begin();
        auto transformsEnd = transforms.end();
        auto graphicsIt = graphics.begin();
        auto graphicsEnd = graphics.end();

        std::size_t count = 0;

        std::scoped_lock lock{transformsLock};
        while (transformIt != transformsEnd && graphicsIt != graphicsEnd) {
            auto &transform = *transformIt;
            auto &graphic = *graphicsIt;

            Id materialId = materialIds.at(graphic.material);
            Id meshId = meshIds.at(graphic.mesh);

            if (count >= drawIds.size()) {
                drawIds.resize(count * 2);
                loadedTransforms.resize(count * 2);
            } else {
                drawIds[count] = {(materialId << MATERIAL_ID_OFFSET) | (meshId << MESH_ID_OFFSET),
                                  Eigen::Translation3f{transform.position} * transform.rotation *
                                  Eigen::AlignedScaling3f{transform.scale}};
            }

            transformIt++;
            graphicsIt++;
        }

        std::ranges::sort(drawIds, std::less(), &std::pair<Id, Eigen::Affine3f>::first);
        std::ranges::copy(drawIds | std::views::transform([](auto pair) { return pair.second; }),
                          std::begin(loadedTransforms));
    }

    void RenderingSystem::loadEntityTransforms() {
        std::scoped_lock lock{transformsLock};

        if (loadedTransforms.size() > transformsBuffer->getSize())
            transformsBuffer->reallocate(loadedTransforms.size() * 2, BufferUsage::STREAM_DRAW);

        transformsBuffer->setSubData(loadedTransforms, 0);
    }

    void RenderingSystem::renderGraphicsEntities() {
        std::scoped_lock lock{transformsLock};
        std::optional<Id> currentMaterialId;
        std::optional<Id> currentMeshId;
        shaderProgram.use();
        shaderProgram.setUniform("projection", perspective(70.0f, 1080.0f / 1920.0f, 0.0, 10.0).matrix());
        shaderProgram.setUniform("view", lookAt({0.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}).matrix());
        std::size_t totalCount = 0;
        std::size_t count = 0;
        for (auto const &[id, transform]: drawIds) {
            Id newMaterialId = (id & (MATERIAL_ID_MAX << MATERIAL_ID_OFFSET)) >> MATERIAL_ID_OFFSET;
            Id newMeshId = (id & (MESH_ID_MAX << MESH_ID_OFFSET) >> MESH_ID_OFFSET);

            if (currentMaterialId != newMaterialId) {
                currentMaterialId = newMaterialId;
                auto &material = materials.at(currentMaterialId.value());
                auto &diffuseMap = textureObjects.at(material.diffuseMap);
                auto &specularMap = textureObjects.at(material.specularMap);
                auto &normalMap = textureObjects.at(material.normalMap);
                auto &heightMap = textureObjects.at(material.heightMap);

                shaderProgram.setUniform("diffuse", material.diffuse);
                shaderProgram.setUniform("specular", material.specular);
                shaderProgram.setUniform("shininess", material.shininess);

                diffuseMap.bindToTextureUnit(0);
                specularMap.bindToTextureUnit(1);
                normalMap.bindToTextureUnit(2);
                heightMap.bindToTextureUnit(3);
            }

            if (!currentMeshId.has_value() || currentMeshId != newMeshId) {
                currentMeshId = newMeshId;

                auto &meshRecord = meshRecords.at(currentMeshId.value());
                auto &vao = meshRecord.vertexArrayObject;
                vao.bindBuffer(transformsBuffer, totalCount - count);

                vao.drawElementsInstanced(DrawMode::TRIANGLES, meshRecord.indicesSize, count, meshRecord.indicesOffset,
                                          meshRecord.verticesOffset);

                count = 0;
            }

            count++;
            totalCount++;
        }
    }

//        gl_area->signal_realize().connect([this, gl_area]() {
//            gl_area->make_current();
//
//            indexBuffer = std::make_unique<IndexBuffer>();
//            vertexBuffer = std::make_unique<VertexBuffer>();
//
//            shader = std::make_unique<Shader>("/home/hiram/Projects/citty/shaders/vertex.vsh",
//                                              "/home/hiram/Projects/citty/shaders/fragment.fsh");
//            glEnable(GL_DEPTH_TEST);
//            glEnable(GL_CULL_FACE);
//            glFrontFace(GL_CCW);
//            glCullFace(GL_FRONT);
//            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//        });
//
////        gl_area->signal_resize().connect([this, gl_area](int width, int height) {
////            gl_area->make_current();
////            setViewportSize(width, height);
////            if (cameraEntity.has_value()) {
////                updateProjectionMatrix(cameraStore->getComponent(cameraEntity.value()));
////            }
////        });
//
//        gl_area->signal_unrealize().connect([this, gl_area]() {
//
//        });
//
//        gl_area->signal_render().connect([this, gl_area](auto context) {
//            gl_area->make_current();
//            render();
//
//            gl_area->queue_draw();
//            return true;
//        }, false);
//
//        gl_area->add_tick_callback([gl_area](auto clock) {
//            gl_area->queue_draw();
//            return G_SOURCE_CONTINUE;
//        });
//
//    }
//
//    void RenderingSystem::setup(citty::ComponentManager &componentManager) {
//        meshStore = componentManager.getComponentStore<MeshRef>();
//        colorStore = componentManager.getComponentStore<Color>();
//        transformStore = componentManager.getComponentStore<Transform>();
//        cameraStore = componentManager.getComponentStore<Camera>();
//
//        meshStore->onComponentCreation([this](EntitySet entities) { onMeshCreate(entities); });
//        colorStore->onComponentCreation([this](EntitySet entities) { onColorCreate(entities); });
//        colorStore->onComponentUpdate([this](EntitySet entities) { onColorUpdate(entities); });
//        transformStore->onComponentCreation([this](EntitySet entities) { onTransformCreate(entities); });
//        transformStore->onComponentUpdate([this](EntitySet entities) { onTransformUpdate(entities); });
//        cameraStore->onComponentCreation([this](EntitySet entities) { onCameraCreate(entities); });
//    }
//
//    void RenderingSystem::render() {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        for (auto &[id, meshRecord]: loadedMeshes) {
//            // Upload mesh records if they are dirty.
//            // Also regenerate buckets
//            if (!meshRecord.dirty) continue;
//            // TODO better implementation of bucket regeneration
//            meshRecord.colorBuffer.upload();
//            meshRecord.matBuffer.upload();
//            meshRecord.dirty = false;
//            regenerateBuckets();
//        }
//
//
//        // Only try to render if there is a camera
//        if (cameraEntity.has_value()) {
//            shader->use();
//
//            // For each bucket, render its contents
//            for (auto const &[bucketIndex, bucketSize]: buckets) {
//                // Leading command for this bucket
//                auto command = *renderCommands.at(bucketIndex);
//                auto const &meshRecord = loadedMeshes.at(command.meshID);
//                meshRecord.arrayObject.bind();
//                glDrawElementsInstanced(GL_TRIANGLES, meshRecord.meshSize, GL_UNSIGNED_INT,
//                                        (const GLvoid *) (meshRecord.indicesIndex * sizeof(uint)),
//                                        bucketSize);
//            }
//        }
//    }
//
//    void RenderingSystem::update(citty::EntityManager &elementManager) {
//    }
//
//    void RenderingSystem::regenerateBuckets() {
//        buckets.clear();
//        auto currentCommand = **renderCommands.begin();
//        long bucketSize = 0;
//        long currentIndex = 0;
//
//        // Sort the render commands before regenerating the buckets
//        std::sort(renderCommands.begin(), renderCommands.end(), DerefLess<RenderCommand>());
//
//        for (auto const &command: renderCommands) {
//            // Lexicographical comparison
//            bool inBucket = currentCommand.bucketWith(*command);
//
//            if (inBucket) {
//                bucketSize++;
//            }
//            if (!inBucket || currentIndex >= renderCommands.size() - 1) {
//
//                buckets.emplace_back(currentIndex, bucketSize);
//
//                currentCommand = *command;
//                bucketSize = 0;
//            }
//            currentIndex++;
//        }
//    }
//
//    void RenderingSystem::stageMeshIntoBuffers(Mesh const &mesh) {
//        MeshRecord meshRecord;
//        meshRecord.arrayObject.bind();
//        vertexBuffer->enableAttribs();
//        indexBuffer->enableAttribs();
//        meshRecord.colorBuffer.enableAttribs();
//        meshRecord.matBuffer.enableAttribs();
//
//        meshRecord.verticesIndex = vertexBuffer->getSize();
//        meshRecord.indicesIndex = indexBuffer->getSize();
//        meshRecord.meshSize = std::ssize(mesh.indices);
//
//        // Shift indices to their actual positions on the buffer
//        auto const &indices = mesh.indices;
//
//        std::vector<GLuint> adjustedIndices;
//
//        std::ranges::transform(indices, std::back_inserter(adjustedIndices), [&meshRecord](GLuint index) {
//            return index + meshRecord.verticesIndex;
//        });
//
//        // Stage the vertices and adjusted indices into their buffers
//        vertexBuffer->addVertices(mesh.vertices);
//        indexBuffer->addIndices(adjustedIndices);
//
//        vertexBuffer->upload();
//        indexBuffer->upload();
//
//        // Store the loaded mesh record into the map
//        loadedMeshes.try_emplace(mesh.ID, std::move(meshRecord));
//    }
//
//    // Try registering the entity into the rendering system, depending on whether the entity
//    // has a mesh, a color and a transform component.
//    void RenderingSystem::tryRegisterEntity(Entity entity) {
//        // TODO find out if this check is necessary
//        // If this entity is already registered, skip it.
//        if (entityRenderMap.contains(entity)) return;
//        // If this entity doesn't have a mesh don't do anything
//        if (!meshStore->hasComponent(entity)) return;
//
//        Mesh const &mesh = meshStore->getComponent(entity);
//        // If this mesh isn't loaded yet, load it
//        if (!loadedMeshes.contains(mesh.ID)) {
//            stageMeshIntoBuffers(mesh);
//        }
//
//        // Don't queue the render if the entity doesn't have a transform or color
//        if (!transformStore->hasComponent(entity) || !colorStore->hasComponent(entity)) return;
//
//        auto color = colorStore->getComponent(entity);
//        auto transform = transformStore->getComponent(entity);
//
//        // Calculate model matrix for this entity
//        auto modelMatrix = transform.calculateMatrix();
//
//        // Load the transform and colors into the matrix and color buffers
//        auto &meshRecord = loadedMeshes.at(mesh.ID);
//        auto &matBuffer = meshRecord.matBuffer;
//        auto &colorBuffer = meshRecord.colorBuffer;
//
//        // Create render command and queue it
//        auto command = std::make_unique<RenderCommand>();
//        command->meshID = mesh.ID;
//        command->bufferPosition = matBuffer.getSize();
//        entityRenderMap.try_emplace(entity, command.get());
//        renderCommands.push_back(std::move(command));
//
//        // Add matrix and color to buffers
//        matBuffer.addModelMat(modelMatrix);
//        colorBuffer.addVertex(color);
//        meshRecord.dirty = true;
//    }
//
//    // TODO implement onMeshUpdate
//    void RenderingSystem::onMeshCreate(EntitySet entities) {
//        for (auto entity: entities) {
//            tryRegisterEntity(entity);
//        }
//    }
//
//    void RenderingSystem::onColorCreate(EntitySet entities) {
//        for (auto entity: entities) {
//            tryRegisterEntity(entity);
//        }
//    }
//
//
//    void RenderingSystem::onTransformCreate(EntitySet entities) {
//        for (auto entity: entities) {
//            if (cameraStore->hasComponent(entity) && cameraEntity.has_value() && entity == cameraEntity.value())
//                updateViewMatrix(transformStore->getComponent(entity));
//
//            tryRegisterEntity(entity);
//        }
//    }
//
//    void RenderingSystem::onTransformUpdate(EntitySet entities) {
//        for (auto entity: entities) {
//            // If this transform is associated with a camera, update the view matrix
//            if (cameraStore->hasComponent(entity) && cameraEntity.has_value() && entity == cameraEntity.value())
//                updateViewMatrix(transformStore->getComponent(entity));
//
//            // If this entity is not rendered, ignore
//            if (!entityRenderMap.contains(entity)) continue;
//
//            auto meshID = entityRenderMap.at(entity)->meshID;
//            auto bufferPosition = entityRenderMap.at(entity)->bufferPosition;
//
//            MeshRecord &meshRecord = loadedMeshes.at(meshID);
//
//            auto newModelMatrix = transformStore->getComponent(entity).calculateMatrix();
//
//            meshRecord.matBuffer.setModelMat(newModelMatrix, bufferPosition);
//
//            meshRecord.dirty = true;
//        }
//    }
//
//    void RenderingSystem::onColorUpdate(EntitySet entities) {
//        for (auto entity: entities) {
//            // If this entity is not rendered, ignore
//            if (!entityRenderMap.contains(entity)) continue;
//
//            auto meshID = entityRenderMap.at(entity)->meshID;
//            auto bufferPosition = entityRenderMap.at(entity)->bufferPosition;
//
//            MeshRecord &meshRecord = loadedMeshes.at(meshID);
//
//            auto newColor = colorStore->getComponent(entity);
//
//            meshRecord.colorBuffer.setVertex(newColor, bufferPosition);
//
//            meshRecord.dirty = true;
//        }
//    }
//
//    void RenderingSystem::onCameraCreate(EntitySet entities) {
//        cameraEntity = *entities.begin();
//
//        Transform cameraTransform{};
//        // If this camera entity has a transform, use it.
//        // Else just use a default transform
//        if (transformStore->hasComponent(cameraEntity.value()))
//            cameraTransform = transformStore->getComponent(cameraEntity.value());
//
//        auto cameraComponent = cameraStore->getComponent(cameraEntity.value());
//
//        // Update matrices
//        updateViewMatrix(cameraTransform);
//        updateProjectionMatrix(cameraComponent);
//    }
//
//    void RenderingSystem::setViewportSize(int width, int height) {
//        RenderingSystem::viewportSize = {width, height};
////        glViewport(0, 0, width, height);
//    }
//
//    void RenderingSystem::updateViewMatrix(Transform const &transform) {
//        // Rotate front and up vectors according to camera's transform
//        auto cameraDirection = transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
//        auto cameraUp = transform.rotation * glm::vec3(0.0f, 0.0f, 1.0f);
//        glm::mat4 view = glm::lookAt(transform.position, transform.position + cameraDirection, cameraUp);
//        shader->use();
//        shader->setMatrix("view", view);
//    }
//
//    void RenderingSystem::updateProjectionMatrix(Camera camera) {
//        glm::mat4 proj = glm::perspective(glm::radians(camera.fov),
//                                          static_cast<float>(viewportSize.first) /
//                                          static_cast<float>(viewportSize.second), 10.0f, 100000.0f);
//        shader->use();
//        shader->setMatrix("projection", proj);
//    }
//
    Eigen::Projective3f perspective(float verticalFoV, float aspectRatio, float zNear, float zFar) {
        if (aspectRatio < 0) throw std::domain_error("aspect ratio cannot be negative");
        if (zFar < zNear) throw std::domain_error("frustrum far cannot be less than frustrum near");
        if (zNear < 0) throw std::domain_error("frustrum near cannot be negative");

        auto projection = Eigen::Projective3f::Identity();
        float tanHalfFoV = std::tan(verticalFoV / 2.0f);

        projection(0, 0) /= aspectRatio * tanHalfFoV;
        projection(1, 1) /= tanHalfFoV;
        projection(2, 2) = -(zFar + zNear) / (zFar - zNear);
        projection(3, 2) = -1.0f;
        projection(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
        projection(0, 0) = 0.0f;
        return projection;
    }

    Eigen::Affine3f
    lookAt(const Eigen::Vector3f &cameraPos, const Eigen::Vector3f &targetPosition, const Eigen::Vector3f &up) {
        Eigen::Vector3f front = (targetPosition - cameraPos).normalized();
        Eigen::Vector3f right = front.cross(up);

//        auto lookAt = Eigen::Affine3f::Identity();
        Eigen::Affine3f lookAt;
        lookAt.matrix() << right.x(), right.y(), right.z(), -right.dot(cameraPos),
                up.x(), up.y(), up.z(), -up.dot(cameraPos),
                -front.x(), -front.y(), -front.z(), front.dot(cameraPos),
                0.0f, 0.0f, 0.0f, 1.0f;

//        lookAt(0, 0) = right.x();
//        lookAt(0, 1) = right.y();
//        lookAt(0, 2) = right.z();
//        lookAt(0, 3) = -right.dot(cameraPos);
//        lookAt(1, 0) = up.x();
//        lookAt(1, 1) = up.y();
//        lookAt(1, 2) = up.z();
//        lookAt(1, 3) = -up.dot(cameraPos);
//        lookAt(2, 0) = -front.x();
//        lookAt(2, 1) = -front.y();
//        lookAt(2, 2) = -front.z();
//        lookAt(2, 3) = front.dot(cameraPos);
        return lookAt;
    }
} // graphics

