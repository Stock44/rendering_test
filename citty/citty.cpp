#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <chrono>
#include <citty/engine/Engine.hpp>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/GraphicsSynchronizationSystem.hpp>
#include <citty/graphics/Renderer.hpp>
#include <citty/graphics/components/PointLight.hpp>
#include <thread>
#include <vector>

#include "graphics/AssimpModelLoader.hpp"

namespace
{
    constexpr auto kTickInterval = std::chrono::duration<float>(1.0f / 240.0f);
} // namespace

int main(int argc, char* argv[])
{
    using namespace citty;

    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "citty", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    engine::Engine engine;
    auto& entityStore = engine.getEntityStore();
    auto& componentStore = engine.getComponentStore();

    graphics::GraphicsEngine graphicsEngine(window);

    auto assimpModelLoader = graphics::AssimpModelLoader(&graphicsEngine, &entityStore);

    auto renderingSystem =
        engine.addSystem<graphics::GraphicsSynchronizationSystem>(window, &graphicsEngine);

    std::jthread engineThread([&engine, &renderingSystem, &entityStore,
            &componentStore, &assimpModelLoader](
        std::stop_token const& stopToken)
        {
            engine.init();

            auto model = assimpModelLoader.loadModel("assets/sponza.obj");

            auto sponzaEntity = assimpModelLoader.buildModelInstance(model);
            // sponza.obj is authored at ~100x the scale of the rest of the scene
            // (raw vertex coordinates in the thousands), so it needs a corrective
            // scale down to land at a realistic, meters-scale atrium size.
            sponzaEntity.getComponent<engine::Transform>().scale =
                Eigen::Vector3f::Constant(0.01f);

            auto carModel = assimpModelLoader.loadModel("assets/bmw.obj");
            auto carEntity = assimpModelLoader.buildModelInstance(carModel);
            {
                auto& carTransform = carEntity.getComponent<engine::Transform>();
                // Middle of the atrium, a few meters ahead of the camera's spawn
                // point so it isn't clipped by the near plane.
                carTransform.position = {8.0f, 0.0f, 0.0f};
                // bmw.obj is authored at ~300x the scale of the rest of the scene
                // (raw vertex coordinates in the hundreds), so it needs the same
                // kind of corrective scale down as sponza.obj above.
                carTransform.scale = Eigen::Vector3f::Constant(0.003f);
            }
            Eigen::Vector3f const carRotationAxis{0.0f, 1.0f, 0.0f};

            struct LightSpec
            {
                float offsetZ;
                Eigen::Vector3f color;
            };
            constexpr float kLightIntensity = 6.0f;
            std::vector<LightSpec> const lightSpecs{
                {-8.0f, kLightIntensity * Eigen::Vector3f{1.0f, 0.0f, 0.0f}}, // red
                {-4.0f, kLightIntensity * Eigen::Vector3f{0.0f, 1.0f, 0.0f}}, // green
                {0.0f, kLightIntensity * Eigen::Vector3f{0.0f, 0.0f, 1.0f}}, // blue
                {4.0f, kLightIntensity * Eigen::Vector3f{1.0f, 1.0f, 0.0f}}, // yellow
                {8.0f, kLightIntensity * Eigen::Vector3f{0.0f, 1.0f, 1.0f}}, // cyan
            };
            for (auto const& lightSpec : lightSpecs)
            {
                auto pointLightEntity = entityStore.newEntity();
                pointLightEntity.addComponent<engine::Transform>(
                    Eigen::Quaternionf::Identity(),
                    Eigen::Vector3f{10.0f, 5.0f, lightSpec.offsetZ},
                    Eigen::Vector3f::Identity());
                pointLightEntity.addComponent<graphics::PointLight>(lightSpec.color,
                                                                    10.0f);
            }

            auto startTime = std::chrono::steady_clock::now();
            while (!stopToken.stop_requested())
            {
                auto tickStart = std::chrono::steady_clock::now();
                auto deltaTime = duration_cast<std::chrono::duration<float, std::milli>>(
                        tickStart - startTime)
                    .count();
                startTime = tickStart;

                engine.update();
                auto rotationDelta = Eigen::Quaternionf(
                    Eigen::AngleAxisf(deltaTime * 0.001f, carRotationAxis));
                carEntity.getComponent<engine::Transform>().rotation *= rotationDelta;

                std::this_thread::sleep_until(
                    tickStart +
                    std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                        kTickInterval));
            }
        });

    graphicsEngine.start();
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
