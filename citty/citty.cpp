#include <thread>
#include <citty/engine/Engine.hpp>
#include <citty/graphics/RenderingSystem.hpp>
#include <citty/graphics/components/Texture.hpp>

#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <citty/engine/components/Transform.hpp>


int main(int argc, char *argv[]) {
    using namespace citty;

    Glib::init();

    auto app = Gtk::Application::create("org.hiram.citty.main");

    auto builder = Gtk::Builder::create_from_file("citty.ui");

    auto glArea = builder->get_widget<Gtk::GLArea>("gl_area");

    engine::Engine engine;

    engine.addSystem<graphics::RenderingSystem>(glArea);

    auto &entityStore = engine.getEntityStore();
    auto &componentStore = engine.getComponentStore();

    auto noTexture = engine::Entity{entityStore.newEntityId(), componentStore};
    noTexture.addComponent<graphics::Texture>("resources/no_texture.png");

    auto testMaterial = engine::Entity{entityStore.newEntityId(), componentStore};
    testMaterial.addComponent<graphics::Material>(Eigen::Vector3f{1.0f, 1.0f, 1.0f},
                                                  Eigen::Vector3f{1.0f, 1.0f, 1.0f}, noTexture, noTexture,
                                                  noTexture, noTexture, 1.0f);

    auto cubeMesh = engine::Entity{entityStore.newEntityId(), componentStore};
    cubeMesh.addComponent<graphics::Mesh>(graphics::Mesh{
            {
                    // vertices (pos, normal, tangent, bitangent, texture)
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
                    //indices
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
    });

    auto testEntity = engine::Entity{entityStore.newEntityId(), componentStore};
    testEntity.addComponent<engine::Transform>(
            Eigen::Quaternionf{Eigen::AngleAxisf(0.0f, Eigen::Vector3f{0.0f, 1.0f, 0.0f})},
            Eigen::Vector3f{15.0f, 0.0f, 0.0f},
            Eigen::Vector3f{1.0f, 1.0f, 1.0f});
    testEntity.addComponent<graphics::Graphics>(cubeMesh, testMaterial);

    auto testEntity2 = engine::Entity{entityStore.newEntityId(), componentStore};
    testEntity2.addComponent<engine::Transform>(
            Eigen::Quaternionf{Eigen::AngleAxisf(0.0f, Eigen::Vector3f{0.0f, 1.0f, 0.0f})},
            Eigen::Vector3f{0.0f, 0.0f, 2.0f},
            Eigen::Vector3f{1.0f, 1.0f, 1.0f},
            testEntity);
    testEntity2.addComponent<graphics::Graphics>(cubeMesh, testMaterial);

    app->signal_activate().connect([&app, &builder, glArea]() {
        auto mainWindow = builder->get_widget<Gtk::Window>("main_window");

        app->add_window(*mainWindow);

        mainWindow->show();

        mainWindow->get_frame_clock()->signal_paint().connect([glArea]() {
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

    std::jthread engineThread([&engine, &testEntity](std::stop_token const &stopToken) {
        Eigen::Vector3f rotation{0.5f, 1.0f, 0.5f};
        rotation.normalize();
        while (!stopToken.stop_requested()) {
            engine.update();
            testEntity.getComponent<engine::Transform>().rotation *= Eigen::Quaternionf(
                    Eigen::AngleAxisf(0.0001f, rotation));
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



