#include <chrono>
#include <iostream>
#include <thread>
#include <citty/Window.hpp>
#include <citty/engine/Engine.hpp>
#include <citty/graphics/RenderingSystem.hpp>
#include <citty/input/InputSystem.hpp>
#include <citty/map/utils.hpp>
#include <citty/map/MapRenderingSystem.hpp>
#include <citty/traffic/components/Target.h>
#include <citty/traffic/PathfindingSystem.h>

#include <gtkmm.h>

int main(int argc, char *argv[]) {
    Glib::init();

    auto app = Gtk::Application::create("org.gtkmm.examples.base");

    auto builder = Gtk::Builder::create_from_file("citty.ui");

    engine::Engine engine;

    std::thread engine_loop([&engine]() {
        engine.run();
    });

    app->signal_activate().connect([&app, &builder, &engine]() {
        auto mainWindow = builder->get_widget<Gtk::Window>("main_window");

        app->add_window(*mainWindow);

        mainWindow->show();

        auto gl_area = builder->get_widget<Gtk::GLArea>("gl_area");

        auto rendering_system = std::make_shared<graphics::RenderingSystem>(gl_area);

        engine.registerSystem(rendering_system);
    });

    app->signal_shutdown().connect([&engine, &engine_loop]() {
        engine.stop();

        engine_loop.join();
    });

    return app->run(argc, argv);
//    using namespace std::chrono;
//    Window window({500, 500});
//
//
//    engine.registerSystem(std::make_unique<input::InputSystem>(window));
//    engine.registerSystem(std::make_unique<map::MapRenderingSystem>());
//    engine.registerSystem(std::make_unique<traffic::PathfindingSystem>());
//
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
//
//    map::loadXMLMap("/home/hiram/Projects/citty/samples/sample_map.osm", componentManager, entityManager);
//    std::cout << "Number of entities in the system: " << entityManager.createEntity() << std::endl;
//    std::cout << "Number of roads: " <<  roadStore->getComponents().size() << std::endl;
//    std::cout << "Number of nodes: " <<  nodeStore->getComponents().size() << std::endl;
//
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



