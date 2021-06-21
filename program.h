#ifndef PROGRAM_H
#define PROGRAM_H

#include "brushes.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

class Program
{
private:
    GLFWwindow *_window = nullptr;
    Brushes brushes;

public:
    static void KeyActionCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void ResizeCallback(GLFWwindow *window, int width, int height);
    static void CursorPosCallback(GLFWwindow *window, double x, double y);
    static void ScrollCallback(GLFWwindow *window, double x, double y);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

protected:
    void onKeyAction(int key, int scancode, int action, int mods);
    void onResize(int width, int height);
    void onMouseMove(int x, int y);
    void onMouseButton(int button, int action, int mods);
    void onScroll(int x, int y);

public:
    Program(GLFWwindow *window);
    virtual ~Program();

    bool SetUp();
    void Render();
    void CleanUp();
};

#endif // PROGRAM_H
