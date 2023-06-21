//
// Created by hiram on 4/11/23.
//

#include <citty/graphics/RenderingEngine.hpp>
#include <Eigen/Geometry>
#include <iterator>
#include <citty/graphics/ShaderProgramBuilder.hpp>
#include <citty/graphics/PointLightEntity.hpp>
#include <citty/graphics/Math.hpp>

namespace citty::graphics {
    RenderingEngine::RenderingEngine() {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);


        // initialize shaders
        ShaderProgramBuilder depthShaderBuilder;
        depthShaderBuilder.addShader({"shaders/depth.vsh", ShaderType::VERTEX});
        depthShaderBuilder.addShader({"shaders/depth.fsh", ShaderType::FRAGMENT});
        depthShaderProgram = depthShaderBuilder.build();

        ShaderProgramBuilder lightAccumulationBuilder;
        lightAccumulationBuilder.addShader({"shaders/lightAccumulation.vsh", ShaderType::VERTEX});
        lightAccumulationBuilder.addShader({"shaders/lightAccumulation.fsh", ShaderType::FRAGMENT});
        lightAccumulationShaderProgram = lightAccumulationBuilder.build();

        ShaderProgramBuilder hdrBuilder;
        hdrBuilder.addShader({"shaders/hdr.vsh", ShaderType::VERTEX});
        hdrBuilder.addShader({"shaders/hdr.fsh", ShaderType::FRAGMENT});
        hdrShaderProgram = hdrBuilder.build();

        ShaderProgramBuilder lightCullingBuilder;
        lightCullingBuilder.addShader({"shaders/lightCulling.csh", ShaderType::COMPUTE});
        lightCullingShaderProgram = lightCullingBuilder.build();

        vertexBuffer = std::make_shared<Buffer<Vertex>>
                (BufferUsage::STATIC_DRAW);
        indexBuffer = std::make_shared<Buffer<unsigned int>>
                (BufferUsage::STATIC_DRAW);

        std::vector<QuadVertex> quadVertices{
                {{-1.0f, 1.0f,  0.0f}, {0.0f, 1.0f}},
                {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
                {{1.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
                {{1.0f,  -1.0f, 0.0f}, {1.0f, 0.0f}},
        };
        hdrQuadBuffer->setData(quadVertices, BufferUsage::STATIC_DRAW);
        hdrQuadVAO.bindBuffer(hdrQuadBuffer);
        hdrQuadVAO.configureAttrib(0, hdrQuadBuffer, 3, AttributeType::FLOAT, false, 0);
        hdrQuadVAO.configureAttrib(1, hdrQuadBuffer, 2, AttributeType::FLOAT, false, offsetof(QuadVertex, texCoords));

        setViewportDimensions(1, 1);

        glClearColor(0.4, 0.4, 0.8, 1);
    }


    RenderingEngine::MeshId RenderingEngine::loadMesh(const Mesh &mesh) {
        auto &meshRecord = meshRecords.emplace_back(
                std::make_shared<Buffer<Eigen::Affine3f >>
                        (BufferUsage::STREAM_DRAW),
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
        vao.configureAttrib(0, vertexBuffer, 3, AttributeType::FLOAT, false, 0);
        vao.configureAttrib(1, vertexBuffer, 3, AttributeType::FLOAT, false, offsetof(Vertex, normal));
        vao.configureAttrib(2, vertexBuffer, 3, AttributeType::FLOAT, false, offsetof(Vertex, tangent));
        vao.configureAttrib(3, vertexBuffer, 3, AttributeType::FLOAT, false, offsetof(Vertex, bitangent));
        vao.configureAttrib(4, vertexBuffer, 2, AttributeType::FLOAT, false, offsetof(Vertex, texCoords));

        vao.setVertexIndicesBuffer(indexBuffer);

        vao.bindBuffer(meshRecord.transformBuffer, 0);
        vao.configureAttrib(5, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 0);
        vao.configureAttrib(6, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 4 * sizeof(float));
        vao.configureAttrib(7, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 8 * sizeof(float));
        vao.configureAttrib(8, meshRecord.transformBuffer, 4, AttributeType::FLOAT, false, 12 * sizeof(float));
        vao.setBufferDivisor(meshRecord.transformBuffer, 1);

        return meshRecords.size() - 1;
    }

    RenderingEngine::TextureId RenderingEngine::loadTexture(Image const &image, TextureSettings settings) {
        auto &texture = materialTextures.emplace_back(image);
        texture.setMagFilter(settings.magFilter);
        texture.setMinFilter(settings.minFilter);
        texture.setSWrapMode(settings.sWrapMode);
        texture.setTWrapMode(settings.tWrapMode);

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

    const std::pair<unsigned int, unsigned int> &RenderingEngine::getViewportDimensions() const {
        return viewportDimensions;
    }

    /**
     * This function updated the render targets' buffer sizes, as well as updating several variables used within the
     * shaders.
     * @param width width of the viewport in pixels
     * @param height height of the viewport in pixels
     */
    void RenderingEngine::setViewportDimensions(unsigned int width, unsigned int height) {
        lightCullingWorkgroupsX = (width + (width % 16)) / 16;
        lightCullingWorkgroupsY = (height + (height % 16)) / 16;
        numberOfTiles = lightCullingWorkgroupsY * lightCullingWorkgroupsY;
        lightCullingShaderProgram.setUniform("screenSize", width, height);

        lightAccumulationShaderProgram.setUniform("numberOfTilesX", lightCullingWorkgroupsX);

        // initialize depth framebuffer
        depthTexture = std::make_shared<Texture2D>(SizedImageFormat::DEPTH_COMPONENT24, width, height);
        depthTexture->setMinFilter(MinFilter::NEAREST);
        depthTexture->setMagFilter(MagFilter::NEAREST);
        depthTexture->setSWrapMode(WrapMode::CLAMP_TO_BORDER);
        depthTexture->setTWrapMode(WrapMode::CLAMP_TO_BORDER);
        depthTexture->setBorderColor({1.0f, 1.0f, 1.0f, 1.0f});

        depthFramebuffer.setDepthAttachment(depthTexture);

        // initialize hdr framebuffer
        hdrColorTexture = std::make_shared<Texture2D>(SizedImageFormat::RGB8, width, height);
        hdrColorTexture->setMinFilter(MinFilter::LINEAR);
        hdrColorTexture->setMagFilter(MagFilter::LINEAR);

        hdrDepthRenderbuffer = std::make_shared<Renderbuffer>(SizedImageFormat::DEPTH_COMPONENT24, width, height);

        hdrFramebuffer.setColorAttachment(hdrColorTexture, 0);
        hdrFramebuffer.setDepthAttachment(hdrDepthRenderbuffer);

        viewportDimensions = {width, height};
    }

    void RenderingEngine::render() {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
        if (renderCommands.empty()) {
            return;
        }

        depthShaderProgram.use();
        depthShaderProgram.setUniform("projection", projection.matrix());
        depthShaderProgram.setUniform("view", view.matrix());

        depthFramebuffer.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        issueDrawCommands();

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        lightCullingShaderProgram.use();
        lightCullingShaderProgram.setUniform("lightCount", pointLightCount);
        lightCullingShaderProgram.setUniform("projection", projection.matrix());
        lightCullingShaderProgram.setUniform("view", view.matrix());
//
        depthTexture->bindToTextureUnit(4);
        lightCullingShaderProgram.setUniform("depthMap", 4);
//
        pointLightsBuffer->bindToTarget(0, BufferTarget::SHADER_STORAGE_BUFFER);
        visiblePointLightIndexBuffer->bindToTarget(1, BufferTarget::SHADER_STORAGE_BUFFER);

        lightCullingShaderProgram.dispatchCompute(lightCullingWorkgroupsX, lightCullingWorkgroupsY, 1);

        Texture::unbindTextureUnit(4);

//        hdrFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightAccumulationShaderProgram.use();
        lightAccumulationShaderProgram.setUniform("projection", projection.matrix());
        lightAccumulationShaderProgram.setUniform("view", view.matrix());
        lightAccumulationShaderProgram.setUniform("viewPosition", viewPosition);

        issueDrawCommands();
//        Framebuffer::unbind();

//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        hdrShaderProgram.use();
//        hdrColorTexture->bindToTextureUnit(0);
//        hdrShaderProgram.setUniform("hdrBuffer", 0);
//        hdrShaderProgram.setUniform("exposure", 1.0f);
//        hdrQuadVAO.draw(DrawMode::TRIANGLE_STRIP, 4);
    }

    void RenderingEngine::setGraphicsEntities(std::span<GraphicsEntity> graphicsEntities) {
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

        lightAccumulationShaderProgram.setUniform("diffuse", material.diffuse);
        lightAccumulationShaderProgram.setUniform("specular", material.specular);
        lightAccumulationShaderProgram.setUniform("shininess", material.shininess);

        diffuseMap.bindToTextureUnit(0);
        lightAccumulationShaderProgram.setUniform("diffuseMap", 0);
        specularMap.bindToTextureUnit(1);
        lightAccumulationShaderProgram.setUniform("specularMap", 1);
        normalMap.bindToTextureUnit(2);
        lightAccumulationShaderProgram.setUniform("normalMap", 2);
        heightMap.bindToTextureUnit(3);
        lightAccumulationShaderProgram.setUniform("heightMap", 3);
    }

    void RenderingEngine::setViewpoint(Eigen::Vector3f const &position, Eigen::Quaternionf const &rotation) {
        viewPosition = position;
        view = lookAt(position, position + rotation * Eigen::Vector3f::UnitX(), rotation * Eigen::Vector3f::UnitY());
    }

    Eigen::Projective3f const &RenderingEngine::getProjection() const {
        return projection;
    }

    void RenderingEngine::setProjection(Eigen::Projective3f const &newProjection) {
        RenderingEngine::projection = newProjection;
    }

    void RenderingEngine::setPointLightEntities(std::span<PointLightEntity> pointLightEntities) {
        if (pointLightEntities.empty()) return;

        if (pointLightsBuffer->getSize() < pointLightEntities.size()) {
            pointLightsBuffer->reallocate(pointLightEntities.size() * 2, BufferUsage::STREAM_DRAW);
            visiblePointLightIndexBuffer->reallocate(pointLightEntities.size() * 2, BufferUsage::STREAM_DRAW);
        }
        pointLightsBuffer->setSubData(pointLightEntities);

        pointLightCount = pointLightEntities.size();
    }

    void RenderingEngine::issueDrawCommands() {
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
    }


} // graphics