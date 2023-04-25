//
// Created by hiram on 4/11/23.
//

#include <citty/graphics/RenderingEngine.hpp>
#include <citty/engine/components/Transform.hpp>
#include <Eigen/Geometry>
#include <iterator>
#include <citty/graphics/ShaderProgramBuilder.hpp>

namespace citty::graphics {
    RenderingEngine::RenderingEngine() {
        ShaderProgramBuilder builder;
        builder.addShader({"shaders/vertex.vsh", ShaderType::VERTEX});
        builder.addShader({"shaders/fragment.fsh", ShaderType::FRAGMENT});

        shaderProgram = builder.build();

        vertexBuffer = std::make_shared<Buffer<Vertex >>
                (BufferUsage::STATIC_DRAW);
        indexBuffer = std::make_shared<Buffer<unsigned int >>
                (BufferUsage::STATIC_DRAW);
        glClearColor(0.4, 0.4, 0.8, 1);
    }


    RenderingEngine::MeshId RenderingEngine::loadMesh(const Mesh &mesh) {
        auto &meshRecord = meshRecords.emplace_back(
                std::make_shared<Buffer<Eigen::Affine3f >>(BufferUsage::STREAM_DRAW),
                std::vector<Eigen::Affine3f>{},
                VertexArray{},
                vertexBuffer->getSize(),
                indexBuffer->getSize(),
                mesh.indices.size()
        );

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

        return meshRecords.size() - 1;
    }

    RenderingEngine::TextureId RenderingEngine::loadTexture(Image const &image, TextureSettings settings) {
        auto &texture = materialTextures.emplace_back();
        texture.setImage(image);
        texture.setTextureMagFilter(settings.magFilter);
        texture.setTextureMinFilter(settings.minFilter);
        texture.setTextureSWrapMode(settings.sWrapMode);
        texture.setTextureTWrapMode(settings.tWrapMode);

        return materialTextures.size() - 1;
    }

    RenderingEngine::MaterialId RenderingEngine::loadMaterial(Material const &material) {
        if (!textureIsLoaded(material.diffuseMap) ||
            !textureIsLoaded(material.specularMap) ||
            !textureIsLoaded(material.normalMap) ||
            !textureIsLoaded(material.heightMap)) {
            throw std::runtime_error("material contains unknown textures");
        }

        materials.emplace_back(material);
        return materials.size() - 1;
    }

    const std::pair<int, int> &RenderingEngine::getViewportDimensions() const {
        return viewportDimensions;
    }

    void RenderingEngine::setViewportDimensions(int width, int height) {
        RenderingEngine::viewportDimensions = {width, height};
    }

    void RenderingEngine::render() {
        glClear(GL_COLOR_BUFFER_BIT);

        if (renderCommands.empty()) {
            glFlush();
            return;
        }

        shaderProgram.use();
        shaderProgram.setUniform("projection", projection.matrix());
        shaderProgram.setUniform("view", view.matrix());

        auto &firstDrawCommand = *renderCommands.begin();
        std::size_t currentMaterial = firstDrawCommand.materialId;
        useMaterial(currentMaterial);
        for (auto const &drawCommand: renderCommands) {
            if (currentMaterial != drawCommand.materialId) {
                currentMaterial = drawCommand.materialId;
                useMaterial(currentMaterial);
            }
            auto &meshRecord = meshRecords.at(drawCommand.meshId);
            auto &vao = meshRecord.vertexArrayObject;
            vao.drawElementsInstanced(DrawMode::TRIANGLES, meshRecord.indicesSize, drawCommand.instanceCount,
                                      meshRecord.indicesOffset, 0, drawCommand.offset);
        }

        glFlush();
    }

    void RenderingEngine::setGraphicsEntities(const std::vector<GraphicsEntity> &graphicsEntities) {
        renderCommands.clear();
        if (graphicsEntities.empty()) return;

        std::vector<std::pair<std::size_t, GraphicsEntity>> keyedGraphicsEntities;
        keyedGraphicsEntities.reserve(graphicsEntities.size());

        std::ranges::transform(graphicsEntities, std::back_inserter(keyedGraphicsEntities),
                               [](GraphicsEntity const &rc) {
                                   return std::make_pair(rc.key(), rc);
                               });

        std::ranges::sort(keyedGraphicsEntities, std::less<>(), &std::pair<std::size_t, GraphicsEntity>::first);

        for (auto &meshRecord: meshRecords) {
            meshRecord.transforms.clear();
        }

        auto &firstGraphicsEntity = keyedGraphicsEntities.begin()->second;
        auto currentMaterial = firstGraphicsEntity.materialId;
        auto currentMesh = firstGraphicsEntity.meshId;
        auto *currentMeshRecord = &meshRecords.at(currentMesh);
        auto bucketSize = 0;
        for (auto &[key, graphicsEntity]: keyedGraphicsEntities) {

            if (graphicsEntity.materialId != currentMaterial || graphicsEntity.meshId != currentMesh) {
                auto offset = currentMeshRecord->transforms.size() - bucketSize;
                renderCommands.emplace_back(bucketSize, offset, currentMaterial, currentMesh);

                currentMaterial = graphicsEntity.materialId;
                currentMesh = graphicsEntity.meshId;
                currentMeshRecord = &meshRecords.at(currentMesh);
                bucketSize = 0;
            }

            currentMeshRecord->transforms.emplace_back(graphicsEntity.transform);
            bucketSize++;
        }

        auto offset = currentMeshRecord->transforms.size() - bucketSize;
        renderCommands.emplace_back(bucketSize, offset, currentMaterial, currentMesh);

        for (auto &meshRecord: meshRecords) {
            auto &transforms = meshRecord.transforms;
            auto &transformBuffer = meshRecord.transformBuffer;
            if (transformBuffer->getSize() < transforms.size()) {
                transformBuffer->reallocate(transforms.size() * 2, BufferUsage::STREAM_DRAW);
            }
            transformBuffer->setSubData(transforms);
        }
    }

    bool RenderingEngine::textureIsLoaded(std::size_t textureId) const {
        return materialTextures.size() > textureId;
    }

    void RenderingEngine::useMaterial(std::size_t materialId) {
        auto &material = materials.at(materialId);
        auto &diffuseMap = materialTextures.at(material.diffuseMap);
        auto &specularMap = materialTextures.at(material.specularMap);
        auto &normalMap = materialTextures.at(material.normalMap);
        auto &heightMap = materialTextures.at(material.heightMap);

        shaderProgram.setUniform("diffuse", material.diffuse);
        shaderProgram.setUniform("specular", material.specular);
        shaderProgram.setUniform("shininess", material.shininess);

        diffuseMap.bindToTextureUnit(0);
        specularMap.bindToTextureUnit(1);
        normalMap.bindToTextureUnit(2);
        heightMap.bindToTextureUnit(3);
    }

    Eigen::Affine3f const &RenderingEngine::getView() const {
        return view;
    }

    void RenderingEngine::setView(Eigen::Affine3f const &newView) {
        RenderingEngine::view = newView;
    }

    Eigen::Projective3f const &RenderingEngine::getProjection() const {
        return projection;
    }

    void RenderingEngine::setProjection(Eigen::Projective3f const &newProjection) {
        RenderingEngine::projection = newProjection;
    }


} // graphics