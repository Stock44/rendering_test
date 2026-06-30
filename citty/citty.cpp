#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <citty/engine/Engine.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/GLFWRenderingSystem.hpp>
#include <citty/graphics/RenderingEngine.hpp>
#include <citty/graphics/components/PointLight.hpp>
#include <thread>

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
    auto modelEntity = renderingSystem->buildModelInstance(model);
    modelEntity.getComponent<engine::Transform>().position = {10.0f, 0.0f,
                                                              0.0f};

    auto pointLightEntity =
        engine::Entity{entityStore.newEntityId(), componentStore};
    pointLightEntity.addComponent<engine::Transform>(
        Eigen::Quaternionf::Identity(), Eigen::Vector3f{7.0f, 0.0f, 0.0f},
        Eigen::Vector3f::Identity());
    pointLightEntity.addComponent<graphics::PointLight>(
        Eigen::Vector3f(10.0f, 1.0f, 1.0f), 20.0f);

    auto startTime = std::chrono::steady_clock::now();
    while (!stopToken.stop_requested()) {
      auto currentTime = std::chrono::steady_clock::now();
      auto deltaTime = duration_cast<std::chrono::duration<float, std::milli>>(
                           currentTime - startTime)
                           .count();
      startTime = currentTime;

      engine.update();
      modelEntity.getComponent<engine::Transform>().rotation *=
          Eigen::Quaternionf(Eigen::AngleAxisf(deltaTime * 0.001f, rotation));
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
