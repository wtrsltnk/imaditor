
#define GLEXTL_IMPLEMENTATION
#include <GL/glextl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "program.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (glfwInit() == GLFW_FALSE)
        return -1;

    GLFWwindow* window = glfwCreateWindow(1024, 768, "IMaditor", NULL, NULL);
    if (window == 0)
    {
        glfwTerminate();
        return -1;
    }

    Program app(window);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    glfwSetKeyCallback(window, Program::KeyActionCallback);
    glfwSetCursorPosCallback(window, Program::CursorPosCallback);
    glfwSetScrollCallback(window, Program::ScrollCallback);

    glfwSetWindowSizeCallback(window, Program::ResizeCallback);
    glfwMakeContextCurrent(window);

    glExtLoadAll((PFNGLGETPROC*)glfwGetProcAddress);

    Program::ResizeCallback(window, 1024, 768);

    if (app.SetUp())
    {
        while (glfwWindowShouldClose(window) == 0)
        {
            glfwWaitEvents();

            glClear(GL_COLOR_BUFFER_BIT);

            app.Render();

            glfwSwapBuffers(window);

        }
        app.CleanUp();
    }

    glfwTerminate();

    return 0;
}
