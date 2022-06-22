//
// Created by hiram on 5/16/22.
//

#include <stdexcept>
#include "Window.h"

Window::Window(std::pair<int, int> size) : size(size) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(size.first, size.second, "cityy", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Store pointer to window object in the glfw window.
    glfwSetWindowUserPointer(window, this);

    // Activate all callbacks
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *windowPtr, int width, int height) {
        auto window = (Window *) glfwGetWindowUserPointer(windowPtr);
        auto callback = window->getViewSizeCallback();
        if (callback) callback(std::make_pair(width, height));
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow *windowPtr, double xPos, double yPos) {
        auto window = (Window *) glfwGetWindowUserPointer(windowPtr);
        auto callback = window->getMouseMoveCallback();
        if (callback) callback(std::make_pair(static_cast<float>(xPos), static_cast<float>(yPos)));
    });

    glfwSetScrollCallback(window, [](GLFWwindow *windowPtr, double xOffset, double yOffset) {
        auto window = (Window *) glfwGetWindowUserPointer(windowPtr);
        auto callback = window->getMouseScrollCallback();
        if (callback) callback(std::make_pair(static_cast<float>(xOffset), static_cast<float>(yOffset)));
    });
}

void Window::useWindowContext() {
    glfwMakeContextCurrent(window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

bool Window::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

std::pair<int, int> Window::getSize() {
    return size;
}

int Window::getKeyState(int key) {
    return glfwGetKey(window, key);
}

void Window::close() {
    glfwSetWindowShouldClose(window, true);
}

const std::function<void(std::pair<int, int>)> &Window::getViewSizeCallback() const {
    return viewSizeCallback;
}

void Window::setViewSizeCallback(const std::function<void(std::pair<int, int>)> &newCallback) {
    viewSizeCallback = newCallback;
}

const std::function<void(std::pair<float, float>)> &Window::getMouseMoveCallback() const {
    return mouseMoveCallback;
}

void Window::setMouseMoveCallback(const std::function<void(std::pair<float, float>)> &newCallback) {
    mouseMoveCallback = newCallback;
}

const std::function<void(std::pair<float, float>)> &Window::getMouseScrollCallback() const {
    return mouseScrollCallback;
}

void Window::setMouseScrollCallback(const std::function<void(std::pair<float, float>)> &newCallback) {
    mouseScrollCallback = newCallback;
}

int Window::getMouseButtonState(int mouseButton) {
    return glfwGetMouseButton(window, mouseButton);
}

