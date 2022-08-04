#include <chrono>
#include <iostream>
#include <thread>
#include "Window.h"
#include "engine/Engine.h"
#include "graphics/RenderingSystem.h"
#include "input/InputSystem.h"
#include "map/utils.h"
#include "map/MapRenderingSystem.h"
#include "traffic/components/Target.h"
#include "traffic/PathfindingSystem.h"

int main() {
    using namespace std::chrono;
    Window window({500, 500});

    engine::Engine engine;

    engine.registerSystem(std::make_unique<graphics::RenderingSystem>(window));
    engine.registerSystem(std::make_unique<input::InputSystem>(window));
    engine.registerSystem(std::make_unique<map::MapRenderingSystem>());
    engine.registerSystem(std::make_unique<traffic::PathfindingSystem>());

    auto &entityManager = engine.getEntityManager();
    auto &componentManager = engine.getComponentManager();

    auto transformStore = componentManager.getComponentStore<Transform>();
    auto cameraStore = componentManager.getComponentStore<graphics::Camera>();
    auto targetStore = componentManager.getComponentStore<traffic::PathfindTarget>();

    auto cameraEntity = entityManager.createEntity();

    auto cameraTransform = Transform();
    transformStore->setComponent(cameraEntity, cameraTransform);
    auto cameraComponent = graphics::Camera();
    cameraStore->setComponent(cameraEntity, cameraComponent);
    auto roadStore = componentManager.getComponentStore<map::Road>();
    auto nodeStore = componentManager.getComponentStore<map::Node>();

    map::loadXMLMap("/home/hiram/Projects/citty/samples/sample_map.osm", componentManager, entityManager);
    std::cout << "Number of entities in the system: " << entityManager.createEntity() << std::endl;
    std::cout << "Number of roads: " <<  roadStore->getComponents().size() << std::endl;
    std::cout << "Number of nodes: " <<  nodeStore->getComponents().size() << std::endl;


    while (!window.shouldWindowClose()) {
        auto last = std::chrono::steady_clock::now();



        engine.update();


        auto delta = steady_clock::now() - last;
//        std::cout << "Frame duration : " << delta << " FPS: " << 1000000000.0 / delta.count()  << std::endl;
        if (delta.count() < 1.0 / 120000000000.0) {
            std::this_thread::sleep_for(nanoseconds(1000000000 / 120) - delta);
        }
    }
}



