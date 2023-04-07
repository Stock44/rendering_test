#include <thread>
#include <citty/engine/Engine.hpp>
#include <citty/graphics/RenderingSystem.hpp>
#include <citty/graphics/components/Texture.hpp>

#include <gtkmm.h>
#include <iostream>
#include <citty/engine/components/Transform.hpp>

int main(int argc, char *argv[]) {
    using namespace citty;

    Glib::init();

    auto app = Gtk::Application::create("org.hiram.citty.main");

    auto builder = Gtk::Builder::create_from_file("citty.ui");

    auto glArea = builder->get_widget<Gtk::GLArea>("gl_area");

    engine::Engine engine;

    auto renderingSystem = engine.addSystem<graphics::RenderingSystem>(glArea);

    auto entity = engine.getEntityStore().newEntityId();
    auto entity2 = engine.getEntityStore().newEntityId();
    auto entity3 = engine.getEntityStore().newEntityId();


    engine.getComponentStore().add<graphics::Texture>(entity, "home.png");
    engine.getComponentStore().add<engine::Transform>(entity);

    engine.getComponentStore().add<graphics::Texture>(entity2, "test1.png");
    engine.getComponentStore().add<graphics::Texture>(entity3, "test2.png");

    app->signal_activate().connect([&app, &builder]() {
        std::cout << "app activated" << std::endl;
        auto mainWindow = builder->get_widget<Gtk::Window>("main_window");

        app->add_window(*mainWindow);

        mainWindow->show();
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

    std::jthread engineThread([&engine](std::stop_token const &stopToken) {
        while (!stopToken.stop_requested()) {
            engine.update();
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



