#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/glextl.h>
#include <GLFW/glfw3.h>

class Program
{
private:
    GLFWwindow* _window;

public:
    static void KeyActionCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void ResizeCallback(GLFWwindow* window, int width, int height);
    static void CursorPosCallback(GLFWwindow* window, double x, double y);
    static void ScrollCallback(GLFWwindow* window, double x, double y);

protected:
    int _display_w, _display_h;

    void onKeyAction(int key, int scancode, int action, int mods);
    void onResize(int width, int height);
    void onMouseMove(int x, int y);
    void onScroll(int x, int y);

public:
    Program(GLFWwindow* window);
    virtual ~Program();

    bool SetUp();
    void Render();
    void CleanUp();

};

#endif // PROGRAM_H
