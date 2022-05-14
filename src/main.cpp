#include "Simulation/Node.h"
#include "Simulation/Road.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/Model.h"
#include "graphics/Object.h"
#include <map>

int main() {
    graphics::GraphicsEngine graphics = graphics::GraphicsEngine(std::make_pair(500, 500));

    std::map<unsigned int, sim::Node> nodes;
    nodes.insert(std::pair(0, sim::Node(0, 0, 0)));
    nodes.insert(std::pair(1, sim::Node(1, 0, 0)));
    nodes.insert(std::pair(2, sim::Node(0, 1, 0)));

    nodes.find(0)->second.addOutgoingRoad(1);
    nodes.find(1)->second.addIncomingRoad(0);

    nodes.find(2)->second.addOutgoingRoad(0);
    nodes.find(0)->second.addIncomingRoad(2);

    nodes.find(1)->second.addOutgoingRoad(2);
    nodes.find(2)->second.addIncomingRoad(1);

    graphics.start();

    std::vector<graphics::Vertex> vertices({
                                                   graphics::Vertex(1.0f, 0.0f, 1.0f),
                                                   graphics::Vertex(-1.0f, 0.0f, 1.0f),
                                                   graphics::Vertex(-1.0f, 0.0f, -1.0f),
                                                   graphics::Vertex(1.0f, 0.0f, 1.0f),
                                                   graphics::Vertex(-1.0f, 0.0f, -1.0f),
                                                   graphics::Vertex(1.0f, 0.0f, -1.0f),
                                           });

    std::vector<graphics::Vertex> triangleVertices({
                                                           graphics::Vertex(0.0f, 0.0f, 0.0f),
                                                           graphics::Vertex(0.0f, 2.0f, 0.0f),
                                                           graphics::Vertex(2.0f, 0.0f, 0.0f),
                                                           graphics::Vertex(0.0f, 0.0f, 2.0f),
                                                   });

    std::vector<uint> triangleIndices({
                                              0, 1, 2,
                                              0, 2, 3,
                                              1, 2, 3,
                                              0, 1, 3,
                                      });

    std::vector<graphics::Vertex> cubeVertices({
                                                       graphics::Vertex(0.0f, 0.0f, 0.0f),
                                                       graphics::Vertex(1.0f, 0.0f, 0.0f),
                                                       graphics::Vertex(1.0f, 1.0f, 0.0f),
                                                       graphics::Vertex(0.0f, 1.0f, 0.0f),
                                                       graphics::Vertex(0.0f, 0.0f, 1.0f),
                                                       graphics::Vertex(1.0f, 0.0f, 1.0f),
                                                       graphics::Vertex(1.0f, 1.0f, 1.0f),
                                                       graphics::Vertex(0.0f, 1.0f, 1.0f),
                                               });

    std::vector<uint> cubeIndices({
                                          0, 1, 2,
                                          0, 2, 3,
                                          1, 6, 2,
                                          1, 5, 6,
                                  });

    auto cube = std::make_shared<graphics::Model>("cube", cubeVertices, cubeIndices);
    auto pyramid = std::make_shared<graphics::Model>("triangle", triangleVertices, triangleIndices);

    auto object1 = std::make_shared<graphics::Object>(cube, glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(0.0f),
                                                      glm::vec3(1.0f, 0.0f, 0.0f));
    auto object2 = std::make_shared<graphics::Object>(pyramid, glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(90.0f),
                                                      glm::vec3(0.0f, 1.0f, 0.0f));
    auto object3 = std::make_shared<graphics::Object>(pyramid, glm::vec3(3.0f, 0.0f, 0.0f), glm::radians(90.0f),
                                                      glm::vec3(1.0f, 0.0f, 0.0f));
    auto object4 = std::make_shared<graphics::Object>(pyramid, glm::vec3(0.0f, 0.0f, -3.0f), glm::radians(90.0f),
                                                      glm::vec3(0.0f, 0.0f, 1.0f));
    auto object5 = std::make_shared<graphics::Object>(cube, glm::vec3(1.0f, -1.0f, 1.0f), glm::radians(0.0f),
                                                      glm::vec3(1.0f, 0.0f, 0.0f));

    graphics.addObject(object2);
    graphics.addObject(object1);
    graphics.addObject(object5);
    graphics.addObject(object3);
    graphics.addObject(object4);

    // TODO extract input logic
    // TODO extract delta time
    // TODO shouldRun() shouldn't be inside graphics, extract.
    while (graphics.shouldRun()) {
        graphics.update();
    }

    return 0;
}