//
// Created by hiram on 5/16/22.
//

#ifndef CITYY_WINDOW_H
#define CITYY_WINDOW_H


#include <utility>
#include <functional>

class Window {
public:
    explicit Window(std::pair<int, int> size);

    void useWindowContext();

    void swapBuffers();

    bool shouldWindowClose();

    std::pair<int, int> getSize();

    int getKeyState(int key);

    int getMouseButtonState(int mouseButton);

    void close();

    [[nodiscard]] const std::function<void(std::pair<int, int>)> &getViewSizeCallback() const;

    void setViewSizeCallback(const std::function<void(std::pair<int, int>)> &newCallback);

    [[nodiscard]] const std::function<void(std::pair<float, float>)> &getMouseMoveCallback() const;

    void setMouseMoveCallback(const std::function<void(std::pair<float, float>)> &newCallback);

    [[nodiscard]] const std::function<void(std::pair<float, float>)> &getMouseScrollCallback() const;

    void setMouseScrollCallback(const std::function<void(std::pair<float, float>)> &newCallback);

private:
    std::pair<int, int> size;

    std::function<void(std::pair<int, int>)> viewSizeCallback;
    std::function<void(std::pair<float, float>)> mouseMoveCallback;
    std::function<void(std::pair<float, float>)> mouseScrollCallback;

    GLFWwindow *window;
};


#endif //CITYY_WINDOW_H
