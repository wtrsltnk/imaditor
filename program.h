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

protected:
    int _display_w, _display_h;

    void onKeyAction(int key, int scancode, int action, int mods);
    void onResize(int width, int height);

public:
    Program(GLFWwindow* window);
    virtual ~Program();

    bool SetUp();
    void Render();
    void CleanUp();

};

#endif // PROGRAM_H
