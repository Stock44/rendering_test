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
            glClearColor(0.4, 0.4, 0.8, 1);
        });

        glArea->signal_render().connect([this, glArea](Glib::RefPtr<Gdk::GLContext> const &glContext) {
            glClear(GL_COLOR_BUFFER_BIT);
            render();
            glFlush();

            return true;
        }, false);

        glArea->signal_resize().connect([this](int width, int height) {
            viewportDimensions = {width, height};
        });

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
            auto &[entity, mesh] = meshLoadingQueue.front();

            MeshRecord meshRecord{
                    std::make_shared<Buffer<Eigen::Affine3f >>(BufferUsage::STREAM_DRAW),
                    std::vector<Eigen::Affine3f>(),
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

            vao.bindBuffer(meshRecord.transformBuffer, 0);
            vao.enableAttrib(5);
            vao.configureAttrib(5, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 0);
            vao.enableAttrib(5);
            vao.configureAttrib(6, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 4 * sizeof(float));
            vao.enableAttrib(5);
            vao.configureAttrib(7, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 8 * sizeof(float));
            vao.enableAttrib(5);
            vao.configureAttrib(8, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 12 * sizeof(float));
            vao.setBufferDivisor(meshRecord.transformBuffer, 1);

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
        for (auto &meshRecord: meshRecords) {
            meshRecord.second.transformData.clear();
        }

        drawIds.clear();
        while (transformIt != transformsEnd && graphicsIt != graphicsEnd) {
            auto &transform = *transformIt;
            auto &graphic = *graphicsIt;

            Id materialId = materialIds.at(graphic.material);
            Id meshId = meshIds.at(graphic.mesh);

            if (!meshRecords.contains(meshId)) continue;
            auto &meshRecord = meshRecords.at(meshId);
            auto &transformData = meshRecord.transformData;

            if (count >= drawIds.size())
                drawIds.resize(count * 2 + 1);
            drawIds[count] = (materialId << MATERIAL_ID_OFFSET) | (meshId << MESH_ID_OFFSET);

            Eigen::Affine3f transformMatrix;

            auto parent = transform.parent;
            if (parent) {
                transformMatrix = Eigen::Affine3f::Identity();
                std::stack<Eigen::Affine3f> transformMatrices;
                transformMatrices.emplace(Eigen::Translation3f{transform.position} * transform.rotation *
                                          Eigen::AlignedScaling3f{transform.scale});
                while (parent) {
                    auto &parentTransform = transform.parent.value().getComponent<engine::Transform>();
                    transformMatrices.emplace(
                            Eigen::Translation3f{parentTransform.position} * parentTransform.rotation *
                            Eigen::AlignedScaling3f{parentTransform.scale});
                    parent = parentTransform.parent;
                }

                while (!transformMatrices.empty()) {
                    transformMatrix = transformMatrix * transformMatrices.top();
                    transformMatrices.pop();
                }
            } else {
                transformMatrix = Eigen::Translation3f(transform.position) * transform.rotation *
                                  Eigen::AlignedScaling3f{transform.scale};
            }

            transformData.emplace_back(std::move(transformMatrix));

            transformIt++;
            graphicsIt++;

        }
        std::ranges::sort(drawIds, std::less());
    }

    void RenderingSystem::loadEntityTransforms() {
        std::scoped_lock lock{transformsLock};

//        if (loadedTransforms.size() > transformsBuffer->getSize())
//            transformsBuffer->reallocate(loadedTransforms.size() * 2, BufferUsage::STREAM_DRAW);
//
//        transformsBuffer->setSubData(loadedTransforms, 0);
    }

    void RenderingSystem::renderGraphicsEntities() {
        std::scoped_lock lock{transformsLock};

        if (drawIds.empty()) return;

        Id currentMaterialId = (drawIds[0] & (MATERIAL_ID_MAX << MATERIAL_ID_OFFSET)) >> MATERIAL_ID_OFFSET;
        Id currentMeshId = (drawIds[0] & (MESH_ID_MAX << MESH_ID_OFFSET) >> MESH_ID_OFFSET);
        shaderProgram.use();
        shaderProgram.setUniform("projection", perspective(70.0f, static_cast<float>(viewportDimensions.first) /
                                                                  static_cast<float>(viewportDimensions.second), 0.0,
                                                           10.0).matrix());
        shaderProgram.setUniform("view",
                                 lookAt({0.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}).matrix());
        std::size_t totalCount = 0;
        std::size_t instanceCount = 0;

        enableMaterial(currentMaterialId);

        for (auto id: drawIds) {
            auto &meshRecord = meshRecords.at(currentMeshId);

            Id newMaterialId = (id & (MATERIAL_ID_MAX << MATERIAL_ID_OFFSET)) >> MATERIAL_ID_OFFSET;
            Id newMeshId = (id & (MESH_ID_MAX << MESH_ID_OFFSET) >> MESH_ID_OFFSET);

            if (currentMaterialId != newMaterialId) {
                currentMaterialId = newMaterialId;
                enableMaterial(currentMaterialId);
            }

            if (currentMeshId != newMeshId) {
                drawMesh(currentMeshId);
            }

            instanceCount++;
            totalCount++;
        }
        drawMesh(currentMeshId);
    }

    void RenderingSystem::drawMesh(RenderingSystem::Id meshId) {
        auto &meshRecord = meshRecords.at(meshId);
        auto &vao = meshRecord.vertexArrayObject;
        auto &transforms = meshRecord.transformData;
        auto &transformBuffer = meshRecord.transformBuffer;

        if (transformBuffer->getSize() < transforms.size()) {
            transformBuffer->reallocate(transforms.size() * 2, BufferUsage::STREAM_DRAW);
        }
        meshRecord.transformBuffer->setSubData(transforms);
        vao.drawElementsInstanced(DrawMode::TRIANGLES, meshRecord.indicesSize, transforms.size(),
                                  meshRecord.indicesOffset,
                                  meshRecord.verticesOffset);
        meshRecords.at(meshId).transformData.clear();
    }

    void RenderingSystem::enableMaterial(RenderingSystem::Id materialId) {
        auto &material = materials.at(materialId);
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
        return projection;
    }

    Eigen::Affine3f
    lookAt(const Eigen::Vector3f &cameraPos, const Eigen::Vector3f &targetPosition, const Eigen::Vector3f &up) {
        Eigen::Vector3f front = (cameraPos - targetPosition).normalized();
        Eigen::Vector3f right = front.cross(up);

        Eigen::Affine3f lookAt;
        lookAt.matrix() << right.x(), right.y(), right.z(), -cameraPos.x(),
                up.x(), up.y(), up.z(), -cameraPos.y(),
                front.x(), front.y(), front.z(), -cameraPos.z(),
                0.0f, 0.0f, 0.0f, 1.0f;

        return lookAt;
    }
} // graphics

