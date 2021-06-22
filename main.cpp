
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "program.h"

using namespace std;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (glfwInit() == GLFW_FALSE)
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "IMaditor", NULL, NULL);
    if (window == 0)
    {
        glfwTerminate();
        return -1;
    }

    Program app(window);

    glfwSetKeyCallback(window, Program::KeyActionCallback);
    glfwSetCursorPosCallback(window, Program::CursorPosCallback);
    glfwSetScrollCallback(window, Program::ScrollCallback);
    glfwSetMouseButtonCallback(window, Program::MouseButtonCallback);

    glfwSetWindowSizeCallback(window, Program::ResizeCallback);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

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
