#include "program.h"
#include <glad/glad.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <nfd.h>

#include "actions/baseaction.h"

#include <IconsFontAwesome4.h>
#include <IconsMaterialDesign.h>

#include "glarraybuffer.h"
#include "glprogram.h"
#include "images.h"
#include "shader.h"
#include "state.h"
#include "tools.h"

#define IMGUI_TABS_IMPLEMENTATION
#include "imgui_tabs.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Tools tools;
Images images;
GlProgram imageShader;
GlProgram backgroundShader;
GlArrayBuffer buffer;

static struct
{
    bool show_toolbar = false;
    bool show_tooloptions = false;
    bool show_content = false;
    bool show_dockbar = false;
    int width = 200;
    int height = 300;
    int mousex = 0;
    int mousey = 0;
    int mouseImagex = 0;
    int mouseImagey = 0;
    int zoom = 100;
    int translatex = 0;
    int translatey = 0;
    bool shiftPressed = false;
    bool ctrlPressed = false;
    glm::vec2 contentPosition;
    glm::vec2 contentSize;
    bool mousePanning = false;

} state;

Program::Program(GLFWwindow *window)
    : _window(window)
{
    glfwSetWindowUserPointer(this->_window, static_cast<void *>(this));
}

Program::~Program()
{
    glfwSetWindowUserPointer(this->_window, nullptr);
}

#define GLSL(src) "#version 150\n" #src

std::string vertexGlsl = GLSL(
    in vec3 vertex;
    in vec2 texcoord;

    uniform mat4 u_projection;
    uniform mat4 u_view;

    out vec2 f_texcoord;

    void main() {
        gl_Position = u_projection * u_view * vec4(vertex.xyz, 1.0);
        f_texcoord = texcoord;
    });

std::string fragmentGlsl = GLSL(
    uniform sampler2D u_texture;

    in vec2 f_texcoord;

    out vec4 color;

    void main() {
        color = texture(u_texture, f_texcoord);
    });

std::string vertexBlocksGlsl = GLSL(
    in vec3 vertex;
    in vec2 texcoord;

    uniform mat4 u_projection;
    uniform mat4 u_view;

    out vec2 f_texcoord;

    void main() {
        gl_Position = u_projection * u_view * vec4(vertex.xyz, 1.0);
        f_texcoord = texcoord;
    });

std::string fragmentBlocksGlsl = GLSL(
    in vec2 f_texcoord;
    out vec4 color;

    void main() {
        if (int(gl_FragCoord.x) % 32 < 16 && int(gl_FragCoord.y) % 32 > 16 || int(gl_FragCoord.x) % 32 > 16 && int(gl_FragCoord.y) % 32 < 16)
            color = vec4(0.9f, 0.9f, 0.92f, 1.0f);
        else
            color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    });

float foreColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float backColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

static size_t selectedTab = 0;
std::vector<std::string> tabNames;
static size_t tabNameAllocCount = 0;
const int dockbarWidth = 250;
const int menubarHeight = 22;
const int optionsbarHeight = 45;
const int statebarHeight = 35;
const int toolboxWidth = 45;

void updateTabNames()
{
    if (images._images.size() >= tabNameAllocCount)
    {
        tabNameAllocCount += 32;
        tabNames.resize(tabNameAllocCount);
    }

    for (size_t i = 0; i < images._images.size(); ++i)
    {
        tabNames[i] = images._images[i].image->_name;
    }
}

void addImage(Image *img)
{
    selectedTab = images._images.size();
    images._images.push_back(img);
    updateTabNames();
    images.select(selectedTab);
}

void newImage()
{
    auto img = new Image();
    img->_name = "New";
    img->_fullPath = "New.png";
    img->addLayer();
    addImage(img);
}

void openImage()
{
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

    if (result == NFD_OKAY)
    {
        auto img = new Image();
        img->_fullPath = outPath;
        std::replace(img->_fullPath.begin(), img->_fullPath.end(), '\\', '/');
        img->_name = img->_fullPath.substr(img->_fullPath.find_last_of('/') + 1);
        img->fromFile(outPath);
        addImage(img);
    }
}

void addLayer()
{
    if (images.selected() != nullptr)
    {
        images.selected()->addLayer();
    }
}

void removeCurrentLayer()
{
    if (images.selected() != nullptr)
    {
        images.selected()->removeCurrentLayer();
    }
}

void moveCurrentLayerUp()
{
    if (images.selected() != nullptr)
    {
        images.selected()->moveCurrentLayerUp();
    }
}

void moveCurrentLayerDown()
{
    if (images.selected() != nullptr)
    {
        images.selected()->moveCurrentLayerDown();
    }
}

bool Program::SetUp()
{
    ImGuiIO &io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);

    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

    static const ImWchar icons_ranges_fontawesome[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FA, 16.0f, &config, icons_ranges_fontawesome);

    static const ImWchar icons_ranges_googleicon[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_MD, 16.0f, &config, icons_ranges_googleicon);

    brushes.init();

    imageShader.init(vertexGlsl.c_str(), fragmentGlsl.c_str());
    backgroundShader.init(vertexBlocksGlsl.c_str(), fragmentBlocksGlsl.c_str());
    buffer.init();

    newImage();

    return true;
}

void Program::Render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glViewport(0, 0, state.width, state.height);
    glClearColor(114 / 255.0f, 144 / 255.0f, 154 / 255.0f, 255 / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (images.selected() != nullptr)
    {
        auto img = images.selected();
        if (img->isDirty()) images.uploadSelectedImage();

        auto zoom = glm::scale(glm::mat4(1.0f), glm::vec3(state.zoom / 100.0f));
        auto translate = glm::translate(zoom, glm::vec3(state.translatex, state.translatey, 0.0f));
        auto scale = glm::scale(translate, glm::vec3(img->_size[0], img->_size[1], 1.0f));

        auto projection = glm::ortho(-(state.width / 2.0f), (state.width / 2.0f), (state.height / 2.0f), -(state.height / 2.0f));

        buffer.bind();

        backgroundShader.bind()
            .matrix("u_projection", projection)
            .matrix("u_view", scale);
        buffer.render();

        imageShader.bind()
            .matrix("u_projection", projection)
            .matrix("u_view", scale);
        buffer.render();

        if (tools.selectedTool()._actionFactory != nullptr &&
            tools.selectedTool()._actionFactory->ToolHelperImage() > 0)
        {
            glBindTexture(GL_TEXTURE_2D, tools.selectedTool()._actionFactory->ToolHelperImage());
            buffer.render();
        }

        buffer.unbind();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4());
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "CTRL+N")) newImage();
                    if (ImGui::MenuItem("Open", "CTRL+O")) openImage();
                    if (ImGui::MenuItem("Save", "CTRL+S"))
                    {
                    }
                    if (ImGui::MenuItem("Save As..", "CTRL+SHIFT+Z"))
                    {
                    }
                    if (ImGui::MenuItem("Close"))
                    {
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Quit"))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Undo", "CTRL+Z"))
                    {
                    }
                    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
                    {
                    } // Disabled item
                    ImGui::Separator();
                    if (ImGui::MenuItem("Cut", "CTRL+X"))
                    {
                    }
                    if (ImGui::MenuItem("Copy", "CTRL+C"))
                    {
                    }
                    if (ImGui::MenuItem("Paste", "CTRL+V"))
                    {
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About IMaditor"))
                    {
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.20f, 0.20f, 0.47f, 0.60f));
        {
            ImGui::Begin("optionsBar", &state.show_tooloptions, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(0, menubarHeight));
                ImGui::SetWindowSize(ImVec2(state.width, optionsbarHeight));

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
                ImGui::Button(ICON_FA_PAW, ImVec2(30, 30));
                ImGui::PopStyleColor(1);
            }
            ImGui::End();

            ImGui::Begin("toolbox", &state.show_toolbar, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(0, menubarHeight + optionsbarHeight));
                ImGui::SetWindowSize(ImVec2(float(toolboxWidth), float(state.height - menubarHeight - optionsbarHeight - statebarHeight)));

                for (size_t i = 0; i < tools.toolCount(); i++)
                {
                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_Button, tools.isSelected(i) ? ImVec4() : ImGui::GetStyle().Colors[ImGuiCol_Button]);
                    if (ImGui::Button(tools[i]._icon, ImVec2(30, 30))) tools.selectTool(i);
                    ImGui::PopStyleColor(1);
                    ImGui::PopID();
                }
            }
            ImGui::End();

            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4());
            {
                ImGui::Begin("content", &(state.show_content), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
                {
                    ImGui::SetWindowPos(ImVec2(state.contentPosition.x, state.contentPosition.y));
                    ImGui::SetWindowSize(ImVec2(state.contentSize.x, state.contentSize.y));

                    if (images.hasImages())
                    {
                        if (ImGui::TabLabels(tabNames, images._images.size(), selectedTab))
                        {
                            images.select(selectedTab);
                        }
                    }
                }
                ImGui::End();
            }
            ImGui::PopStyleColor();

            ImGui::Begin("dockbar", &(state.show_dockbar), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(float(state.width - dockbarWidth), float(menubarHeight + optionsbarHeight)));
                ImGui::SetWindowSize(ImVec2(float(dockbarWidth), float(state.height - menubarHeight - optionsbarHeight - statebarHeight)));

                static bool colorOptionsOpen = true;
                if (ImGui::CollapsingHeader("Color options", &colorOptionsOpen, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    static int e = 0;
                    ImGui::RadioButton("Fore", &e, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("Back", &e, 1);
                    if (e == 0)
                    {
                        ImGui::ColorPicker("rgb", foreColor);
                        ImGui::SliderFloat("alpha", &foreColor[3], 0.0f, 1.0f);
                    }
                    else
                    {
                        ImGui::ColorPicker("rgb", backColor);
                        ImGui::SliderFloat("alpha", &backColor[3], 0.0f, 1.0f);
                    }
                }

                if (images.hasImages())
                {
                    static bool layerOptionsOpen = true;
                    if (ImGui::CollapsingHeader("Layer options", &layerOptionsOpen, ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::Button(ICON_FA_PLUS, ImVec2(30.0f, 30.0f))) addLayer();
                        ImGui::SameLine();
                        if (ImGui::Button(ICON_FA_MINUS, ImVec2(30.0f, 30.0f))) removeCurrentLayer();
                        ImGui::SameLine();
                        if (ImGui::Button(ICON_FA_ARROW_UP, ImVec2(30.0f, 30.0f))) moveCurrentLayerUp();
                        ImGui::SameLine();
                        if (ImGui::Button(ICON_FA_ARROW_DOWN, ImVec2(30.0f, 30.0f))) moveCurrentLayerDown();

                        ImGui::Separator();

                        for (size_t i = 0; i < images.selected()->_layers.size(); i++)
                        {
                            auto layer = images.selected()->_layers[i];
                            ImGui::PushID(i);
                            auto title = layer->_name;
                            if (images.selected()->_selectedLayer == i) title += " (selected)";
                            if (ImGui::TreeNode("layer_node", "%s", title.c_str()))
                            {
                                if (ImGui::Button(layer->isVisible() ? ICON_FA_EYE : ICON_FA_EYE_SLASH, ImVec2(30, 30)))
                                {
                                    layer->toggleVisibility();
                                }
                                ImGui::SameLine();
                                ImGui::PushStyleColor(ImGuiCol_Button, i == images.selected()->_selectedLayer ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.20f, 0.40f, 0.47f, 0.0f));
                                if (ImGui::Button(layer->_name.c_str(), ImVec2(-1, 30))) images.selected()->selectLayer(i);
                                ImGui::PopStyleColor(1);

                                if (ImGui::SliderFloat("Alpha", &(layer->_alpha), 0.0f, 1.0f))
                                {
                                    layer->setDirty();
                                }
                                if (ImGui::Combo("Mode", &(layer->_alphaMode), "Normal\0Darken\0Lighten\0Hue\0Saturation\0Color\0Lumminance\0Multiply\0Screen\0Dissolve\0Overlay\0Hard Light\0Soft Light\0Difference\0Dodge\0Burn\0Exclusion\0\0"))
                                {
                                    layer->setDirty();
                                }

                                ImGui::TreePop();
                            }
                            ImGui::PopID();
                        }

                        ImGui::Separator();
                    }
                }

                static bool brushOptionsOpen = true;
                if (ImGui::CollapsingHeader("Brush options", &brushOptionsOpen, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (int i = 0; i < brushes._count; ++i)
                    {
                        auto textureId = reinterpret_cast<ImTextureID>(brushes._brushes[i]._textureIndex);
                        ImGui::ImageButton(textureId, ImVec2(30, 30));
                    }
                    ImGui::Separator();
                }
            }
            ImGui::End();

            ImGui::Begin("statusbar", &(state.show_content), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(0.0f, float(state.height - statebarHeight)));
                ImGui::SetWindowSize(ImVec2(float(state.width), float(statebarHeight)));

                ImGui::Columns(3);
                ImGui::Text("status bar");
                ImGui::NextColumn();
                ImGui::SliderInt("zoom", &(state.zoom), 10, 400);
                ImGui::NextColumn();
                ImGui::Text("mouse: %d %d", state.mousex, state.mousey);
                ImGui::Columns(1);
            }
            ImGui::End();
        }
        ImGui::PopStyleColor();
    }
    ImGui::PopStyleVar();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Program::onKeyAction(int key, int scancode, int action, int mods)
{
    (void)key;
    (void)scancode;
    (void)action;

    state.shiftPressed = (mods & GLFW_MOD_SHIFT);
    state.ctrlPressed = (mods & GLFW_MOD_CONTROL);
}

bool isMouseInContent()
{
    if (state.mousex < state.contentPosition.x) return false;
    if (state.mousey < state.contentPosition.y) return false;
    if (state.mousex > (state.contentPosition.x + state.contentSize.x)) return false;
    if (state.mousey > (state.contentPosition.y + state.contentSize.y)) return false;

    return true;
}

void Program::onMouseMove(int x, int y)
{
    state.mousex = x;
    state.mousey = y;

    if (tools.selectedTool()._actionFactory == nullptr) return;

    auto fac = tools.selectedTool()._actionFactory;

    if (!isMouseInContent()) return;

    if (images.selected() != nullptr)
    {
        auto img = images.selected();
        auto zoom = glm::scale(glm::mat4(1.0f), glm::vec3(state.zoom / 100.0f));
        auto translate = glm::translate(zoom, glm::vec3(state.translatex, -state.translatey, 0.0f));
        auto projection = glm::ortho(-(state.width / 2.0f), (state.width / 2.0f), (state.height / 2.0f), -(state.height / 2.0f));

        auto pp = glm::unProject(glm::vec3(x, y, 0.0f),
                                 translate, projection,
                                 glm::vec4(0.0f, 0.0f, state.width, state.height));

        state.mouseImagex = pp.x + (img->_size[0] / 2.0f);
        state.mouseImagey = -(pp.y - (img->_size[1] / 2.0f));

        fac->MouseMove(images.selected(), state.mouseImagex, state.mouseImagey);
    }
}

void Program::onMouseButton(int button, int action, int mods)
{
    state.mousePanning = false;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && mods & GLFW_MOD_SHIFT)
    {
        state.mousePanning = true;
        return;
    }

    if (images.selected() == nullptr) return;

    if (tools.selectedTool()._actionFactory == nullptr) return;

    if (!isMouseInContent()) return;

    auto fac = tools.selectedTool()._actionFactory;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        fac->PrimaryMouseButtonDown(images.selected(),
                                    mods & GLFW_MOD_SHIFT,
                                    mods & GLFW_MOD_CONTROL,
                                    mods & GLFW_MOD_ALT,
                                    mods & GLFW_MOD_SUPER);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        fac->PrimaryMouseButtonUp(images.selected(),
                                  mods & GLFW_MOD_SHIFT,
                                  mods & GLFW_MOD_CONTROL,
                                  mods & GLFW_MOD_ALT,
                                  mods & GLFW_MOD_SUPER);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        fac->SecondaryMouseButtonUp(images.selected(),
                                    mods & GLFW_MOD_SHIFT,
                                    mods & GLFW_MOD_CONTROL,
                                    mods & GLFW_MOD_ALT,
                                    mods & GLFW_MOD_SUPER);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        fac->SecondaryMouseButtonUp(images.selected(),
                                    mods & GLFW_MOD_SHIFT,
                                    mods & GLFW_MOD_CONTROL,
                                    mods & GLFW_MOD_ALT,
                                    mods & GLFW_MOD_SUPER);
    }
}

void Program::onScroll(int x, int y)
{
    (void)x;

    if (state.shiftPressed)
    {
        state.translatex += (y * 5);
    }
    else if (state.ctrlPressed)
    {
        state.translatey += (y * 5);
    }
    else
    {
        state.zoom += (y * 5);
        if (state.zoom < 10) state.zoom = 10;
    }
}

void Program::onResize(int width, int height)
{
    state.width = width;
    state.height = height;
    state.contentPosition = glm::vec2(toolboxWidth, menubarHeight + optionsbarHeight);
    state.contentSize = glm::vec2(state.width - toolboxWidth - dockbarWidth, state.height - menubarHeight - optionsbarHeight);

    glViewport(0, 0, width, height);
}

void Program::CleanUp()
{}

void Program::KeyActionCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onKeyAction(key, scancode, action, mods);
}

void Program::CursorPosCallback(GLFWwindow *window, double x, double y)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onMouseMove(int(x), int(y));
}

void Program::ScrollCallback(GLFWwindow *window, double x, double y)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onScroll(int(x), int(y));
}

void Program::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onMouseButton(button, action, mods);
}

void Program::ResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = static_cast<Program *>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onResize(width, height);
}
