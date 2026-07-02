#include <ranges>
//
// Created by hiram on 6/6/22.
//

#include <Eigen/Geometry>
#include <assimp/postprocess.h>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/GraphicsSynchronizationSystem.hpp>
#include <citty/graphics/ShaderProgramBuilder.hpp>
#include <iterator>
#include <ranges>
#include <unordered_set>


namespace citty::graphics
{
  GraphicsSynchronizationSystem::GraphicsSynchronizationSystem(GLFWwindow* window, GraphicsEngine* engine)
    : window(window), graphicsEngine(engine)
  {
  }


  void GraphicsSynchronizationSystem::init()
  {
  }

  void GraphicsSynchronizationSystem::update()
  {
    handleGraphicsEntities();
    handlePointLightEntities();
  }

  void GraphicsSynchronizationSystem::handleGraphicsEntities()
  {
    auto components = getComponents<engine::Transform, Graphics>();

    std::vector<GraphicsEntity> newGraphicEntities;

    std::ranges::transform(
      components, std::back_inserter(newGraphicEntities), [](auto components)
      {
        auto const& [transform, graphic] = components;
        Eigen::Affine3f transformMatrix;
        if (transform.parent)
        {
          transformMatrix = Eigen::Affine3f::Identity();
          std::stack<Eigen::Affine3f> transformMatrices;
          transformMatrices.emplace(Eigen::Translation3f{transform.position} *
            transform.rotation *
            Eigen::AlignedScaling3f{transform.scale});
          std::optional<engine::Entity> currentEntity = transform.parent;
          std::unordered_set<engine::Entity> visitedAncestors;
          while (currentEntity)
          {
            if (visitedAncestors.contains(currentEntity.value()))
              throw std::runtime_error("error: transform loop detected");
            visitedAncestors.insert(currentEntity.value());

            engine::Transform const& currentTransform =
              currentEntity.value().getComponent<engine::Transform>();
            transformMatrices.emplace(
              Eigen::Translation3f{currentTransform.position} *
              currentTransform.rotation *
              Eigen::AlignedScaling3f{currentTransform.scale});
            currentEntity = currentTransform.parent;
          }

          while (!transformMatrices.empty())
          {
            transformMatrix = transformMatrix * transformMatrices.top();
            transformMatrices.pop();
          }
        }
        else
        {
          transformMatrix = Eigen::Translation3f(transform.position) *
            transform.rotation *
            Eigen::AlignedScaling3f{transform.scale};
        }
        return GraphicsEntity{transformMatrix, graphic.material, graphic.mesh};
      });

    graphicsEngine->swapGraphicEntities(newGraphicEntities);
  }

  void GraphicsSynchronizationSystem::handlePointLightEntities()
  {
    auto components = getComponents<engine::Transform, PointLight>();

    std::vector<PointLightEntity> newPointLightEntities;
    for (auto const& [transform, pointLight] : components )
    {
      Eigen::Vector4f position, color;
      position << transform.position, 1.0f;
      color << pointLight.color, 1.0f;
      newPointLightEntities.emplace_back(position, color, pointLight.radius);
    }

    graphicsEngine->swapPointLightEntities(newPointLightEntities);
  }
} // namespace citty::graphics
