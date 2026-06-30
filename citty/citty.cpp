#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <chrono>
#include <citty/engine/Engine.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/GLFWRenderingSystem.hpp>
#include <citty/graphics/RenderingEngine.hpp>
#include <citty/graphics/components/PointLight.hpp>
#include <thread>
#include <vector>

namespace {
constexpr auto kTickInterval = std::chrono::duration<float>(1.0f / 240.0f);
} // namespace

int main(int argc, char *argv[]) {
  using namespace citty;

  if (!glfwInit())
    return -1;

  GLFWwindow *window = glfwCreateWindow(640, 480, "citty", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    return 1;
  }

  engine::Engine engine;
  auto &entityStore = engine.getEntityStore();
  auto &componentStore = engine.getComponentStore();

  auto renderingSystem =
      engine.addSystem<graphics::GLFWRenderingSystem>(window);

  std::jthread engineThread([&engine, &renderingSystem, &entityStore,
                             &componentStore](
                                std::stop_token const &stopToken) {
    engine.init();
    Eigen::Vector3f rotation{0.0f, 1.0f, 0.0f};
    rotation.normalize();

    auto model = renderingSystem->loadModel("assets/car.obj");

    // Camera looks down +X with +Y up, so spreading along Z fans the cars
    // out across the screen left-to-right.
    std::vector<float> const carOffsetsZ{-8.0f, -4.0f, 0.0f, 4.0f, 8.0f};
    std::vector<engine::Entity> carEntities;
    for (float offsetZ : carOffsetsZ) {
      auto carEntity = renderingSystem->buildModelInstance(model);
      carEntity.getComponent<engine::Transform>().position = {16.0f, 0.0f,
                                                               offsetZ};
      carEntities.push_back(carEntity);
    }

    struct LightSpec {
      float offsetZ;
      Eigen::Vector3f color;
    };
    constexpr float kLightIntensity = 6.0f;
    std::vector<LightSpec> const lightSpecs{
        {-8.0f, kLightIntensity * Eigen::Vector3f{1.0f, 0.0f, 0.0f}}, // red
        {-4.0f, kLightIntensity * Eigen::Vector3f{0.0f, 1.0f, 0.0f}}, // green
        {0.0f, kLightIntensity * Eigen::Vector3f{0.0f, 0.0f, 1.0f}},  // blue
        {4.0f, kLightIntensity * Eigen::Vector3f{1.0f, 1.0f, 0.0f}},  // yellow
        {8.0f, kLightIntensity * Eigen::Vector3f{0.0f, 1.0f, 1.0f}},  // cyan
    };
    for (auto const &lightSpec : lightSpecs) {
      auto pointLightEntity =
          engine::Entity{entityStore.newEntityId(), componentStore};
      pointLightEntity.addComponent<engine::Transform>(
          Eigen::Quaternionf::Identity(),
          Eigen::Vector3f{10.0f, 5.0f, lightSpec.offsetZ},
          Eigen::Vector3f::Identity());
      pointLightEntity.addComponent<graphics::PointLight>(lightSpec.color,
                                                           10.0f);
    }

    auto startTime = std::chrono::steady_clock::now();
    while (!stopToken.stop_requested()) {
      auto tickStart = std::chrono::steady_clock::now();
      auto deltaTime = duration_cast<std::chrono::duration<float, std::milli>>(
                           tickStart - startTime)
                           .count();
      startTime = tickStart;

      engine.update();
      auto rotationDelta =
          Eigen::Quaternionf(Eigen::AngleAxisf(deltaTime * 0.001f, rotation));
      for (auto &carEntity : carEntities) {
        carEntity.getComponent<engine::Transform>().rotation *= rotationDelta;
      }

      std::this_thread::sleep_until(
          tickStart + std::chrono::duration_cast<
                          std::chrono::steady_clock::duration>(kTickInterval));
    }
  });

  renderingSystem->start();
  return 0;

  //
  //    while (!window.shouldWindowClose()) {
  //        auto last = std::chrono::steady_clock::now();
  //
  //
  //        engine.update();
  //
  //
  //
  //        auto delta = steady_clock::now() - last;
  ////        std::cout << "Frame duration : " << delta << " FPS: " <<
  /// 1000000000.0 / delta.count()  << std::endl;
  //        if (delta.count() < 1.0 / 120000000000.0) {
  //            std::this_thread::sleep_for(nanoseconds(1000000000 / 120) -
  //            delta);
  //        }
  //    }
}
