#include <thread>
#include <citty/engine/Engine.hpp>
#include <citty/graphics/RenderingSystem.hpp>
#include <citty/graphics/RenderingEngine.hpp>
#include <gtkmm-4.0/gtkmm.h>
#include <citty/engine/components/Transform.hpp>
#include <citty/graphics/components/PointLight.hpp>
#include <chrono>


int main(int argc, char *argv[]) {
    using namespace citty;

    Glib::init();

    auto app = Gtk::Application::create("org.hiram.citty.main");

    auto builder = Gtk::Builder::create_from_file("citty.ui");

    auto glArea = builder->get_widget<Gtk::GLArea>("gl_area");

    engine::Engine engine;
    auto &entityStore = engine.getEntityStore();
    auto &componentStore = engine.getComponentStore();

    auto renderingSystem = engine.addSystem<graphics::RenderingSystem>(glArea);

    using std::chrono::steady_clock;
    app->signal_activate().connect(
            [&app, &builder, glArea]() {
                auto mainWindow = builder->get_widget<Gtk::Window>("main_window");

                app->add_window(*mainWindow);

                mainWindow->show();

                mainWindow->get_frame_clock()->signal_paint().connect([mainWindow, glArea]() {
                    double fps = mainWindow->get_frame_clock()->get_fps();
                    std::cout << fps << " fps\n";
                    glArea->queue_draw();
                });
            });

//    engine.registerSystem(std::make_unique<input::InputSystem>(window));
//    engine.registerSystem(std::make_unique<map::MapRenderingSystem>());
//    engine.registerSystem(std::make_unique<traffic::PathfindingSystem>());

//    auto &entityManager = engine.getEntityManager();
//    auto &componentManager = engine.getComponentManager();
//
//    auto transformStore = componentManager.getComponentStore<Transform>();
//    auto cameraStore = componentManager.getComponentStore<graphics::Camera>();
//    auto targetStore = componentManager.getComponentStore<traffic::PathfindTarget>();
//
//    auto cameraEntity = entityManager.createEntity();
//
//    auto cameraTransform = Transform();
//    transformStore->setComponent(cameraEntity, cameraTransform);
//    auto cameraComponent = graphics::Camera();
//    cameraStore->setComponent(cameraEntity, cameraComponent);
//    auto roadStore = componentManager.getComponentStore<map::Road>();
//    auto nodeStore = componentManager.getComponentStore<map::Node>();

//    map::loadXMLMap("/home/hiram/Projects/citty/samples/sample_map.osm", componentManager, entityManager);
//    std::cout << "Number of entities in the system: " << entityManager.createEntity() << std::endl;
//    std::cout << "Number of roads: " << roadStore->getComponents().size() << std::endl;
//    std::cout << "Number of nodes: " << nodeStore->getComponents().size() << std::endl;

    std::jthread engineThread(
            [&engine, &renderingSystem, &entityStore, &componentStore](std::stop_token const &stopToken) {
                engine.init();
                Eigen::Vector3f rotation{0.0f, 1.0f, 0.0f};
                rotation.normalize();
                auto emptyTextureId = renderingSystem->loadTexture("resources/no_texture.png",
                                                                   graphics::TextureSettings{});
                graphics::Material testMaterial{
                        Eigen::Vector3f{1.0f, 1.0f, 1.0f},
                        Eigen::Vector3f{1.0f, 1.0f, 1.0f},
                        emptyTextureId,
                        emptyTextureId,
                        emptyTextureId,
                        emptyTextureId,
                        1.0f
                };
                auto testMaterialId = renderingSystem->loadMaterial(testMaterial);

                graphics::Mesh cubeMesh{
                        {
//                                 vertices (pos, normal, tangent, bitangent, texture)
                                {{0.0f, 0.0f, 0.0f}, {}, {}, {}, {}},
                                {{1.0f, 0.0f, 0.0f}, {}, {}, {}, {}},
                                {{0.0f, 1.0f, 0.0f}, {}, {}, {}, {}},
                                {{1.0f, 1.0f, 0.0f}, {}, {}, {}, {}},
                                {{0.0f, 0.0f, 1.0f}, {}, {}, {}, {}},
                                {{1.0f, 0.0f, 1.0f}, {}, {}, {}, {}},
                                {{0.0f, 1.0f, 1.0f}, {}, {}, {}, {}},
                                {{1.0f, 1.0f, 1.0f}, {}, {}, {}, {}},
                        },
                        {
//                                indices
                                0, 1, 3,
                                0, 3, 2,
                                2, 3, 7,
                                2, 7, 6,
                                4, 0, 2,
                                4, 2, 6,
                                1, 5, 7,
                                1, 7, 3,
                                4, 5, 1,
                                4, 1, 0,
                                5, 4, 6,
                                5, 6, 7,
                        }
                };
                graphics::Mesh pyramidMesh{
                        {
//                                 vertices (pos, normal, tangent, bitangent, texture)
                                {{0.0f, 0.0f, 0.0f}, {}, {}, {}, {}},
                                {{1.0f, 0.0f, 0.0f}, {}, {}, {}, {}},
                                {{0.5f, 0.0f, 1.0f}, {}, {}, {}, {}},
                                {{0.5f, 1.0f, 0.5f}, {}, {}, {}, {}},
                        },
                        {
//                                indices
                                0, 2, 3,
                                1, 0, 3,
                                2, 1, 3,
                                1, 2, 0,
                        }
                };
                auto cubeMeshId = renderingSystem->loadMesh(cubeMesh);
                auto pyramidMeshId = renderingSystem->loadMesh(pyramidMesh);

                auto testEntity = engine::Entity{entityStore.newEntityId(), componentStore};
                testEntity.addComponent<engine::Transform>(
                        Eigen::Quaternionf::Identity(),
                        Eigen::Vector3f{10.0f, 0.0f, 0.0f});
                testEntity.addComponent<graphics::Graphics>(cubeMeshId, testMaterialId);

                auto testEntity2 = engine::Entity{entityStore.newEntityId(), componentStore};
                testEntity2.addComponent<engine::Transform>(
                        Eigen::Quaternionf::Identity(),
                        Eigen::Vector3f{0.0f, 0.0f, -5.0f},
                        Eigen::Vector3f{1.0f, 1.0f, 1.0f},
                        testEntity);
                testEntity2.addComponent<graphics::Graphics>(pyramidMeshId, testMaterialId);

                auto testEntity3 = engine::Entity{entityStore.newEntityId(), componentStore};
                testEntity3.addComponent<engine::Transform>(
                        Eigen::Quaternionf::Identity(),
                        Eigen::Vector3f{0.0f, 2.0f, 0.0f},
                        Eigen::Vector3f{1.0f, 1.0f, 1.0f},
                        testEntity2);
                testEntity3.addComponent<graphics::Graphics>(cubeMeshId, testMaterialId);

                auto model = renderingSystem->loadModel("assets/car.obj");
                auto modelEntity = renderingSystem->buildModelInstance(model);
                modelEntity.getComponent<engine::Transform>().position = {10.0f, -3.0f, 0.0f};

                auto pointLightEntity = engine::Entity{entityStore.newEntityId(), componentStore};
                pointLightEntity.addComponent<engine::Transform>(
                        Eigen::Quaternionf::Identity(),
                        Eigen::Vector3f{7.0f, 0.0f, 0.0f},
                        Eigen::Vector3f::Identity()
                );
                pointLightEntity.addComponent<graphics::PointLight>(Eigen::Vector3f(1.0f, 1.0f, 1.0f), 20.0f);

                auto startTime = std::chrono::steady_clock::now();
                while (!stopToken.stop_requested()) {
                    auto currentTime = std::chrono::steady_clock::now();
                    auto deltaTime = duration_cast<std::chrono::duration<float, std::milli>>(
                            currentTime - startTime).count();
                    startTime = currentTime;

                    engine.update();
                    modelEntity.getComponent<engine::Transform>().rotation *= Eigen::Quaternionf(
                            Eigen::AngleAxisf(deltaTime * 0.001f, rotation));
                    testEntity.getComponent<engine::Transform>().rotation *= Eigen::Quaternionf(
                            Eigen::AngleAxisf(deltaTime * 0.001f, rotation));

                }
            });

    return app->run(argc, argv);

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
////        std::cout << "Frame duration : " << delta << " FPS: " << 1000000000.0 / delta.count()  << std::endl;
//        if (delta.count() < 1.0 / 120000000000.0) {
//            std::this_thread::sleep_for(nanoseconds(1000000000 / 120) - delta);
//        }
//    }
}



