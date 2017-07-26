#include "program.h"
#include <GL/glextl.h>
#include <nfd.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_impl_glfw_gl3.h"

#define IMGUI_TABS_IMPLEMENTATION
#include "imgui_tabs.h"
#include "font-icons.h"
#include "shader.h"
#include "tools.h"
#include "actions/baseaction.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static Tools tools;

struct {
    bool show_toolbar = false;
    bool show_inspector = false;
    bool show_smartcontrols = false;
    bool show_editors = false;
    bool show_mixer = false;
    bool show_listeditor = false;
    bool show_notes = false;
    bool show_loops = false;
    bool show_browser = false;
    float w = 200.0f;
    float h = 300.0f;
    int mousex;
    int mousey;
    int zoom = 100;
    int translatex = 0;
    int translatey = 0;
    bool shiftPressed = false;
    bool ctrlPressed = false;

} windowConfig;

Program::Program(GLFWwindow* window)
    : _window(window), _display_w(1024), _display_h(768)
{
    glfwSetWindowUserPointer(this->_window, static_cast<void*>(this));
}

Program::~Program()
{
    glfwSetWindowUserPointer(this->_window, nullptr);
}

static std::string vertexGlsl = "#version 150\n\
        in vec3 vertex;\
in vec2 texcoord;\
\
uniform mat4 u_projection;\
uniform mat4 u_view;\
\
out vec2 f_texcoord;\
\
void main()\
{\
    gl_Position = u_projection * u_view * vec4(vertex.xyz, 1.0);\
    f_texcoord = texcoord;\
}";

static std::string fragmentGlsl = "#version 150\n\
        uniform sampler2D u_texture;\
\
in vec2 f_texcoord;\
\
out vec4 color;\
\
void main()\
{\
    color = texture(u_texture, f_texcoord);\
}";




static std::string vertexBlocksGlsl = "#version 150\n\
        in vec3 vertex;\
in vec2 texcoord;\
\
uniform mat4 u_projection;\
\
out vec2 f_texcoord;\
\
void main()\
{\
    gl_Position = u_projection * vec4(vertex.xyz, 1.0);\
    f_texcoord = texcoord;\
}";

static std::string fragmentBlocksGlsl = "#version 150\n\
        \
        in vec2 f_texcoord;\
out vec4 color;\
\
void main()\
{\
    if (int(gl_FragCoord.x) % 32 < 16 && int(gl_FragCoord.y) % 32 > 16\
            || int(gl_FragCoord.x) % 32 > 16 && int(gl_FragCoord.y) % 32 < 16)\
        color = vec4(0.9f, 0.9f, 0.92f, 1.0f);\
    else\
        color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\
}";

static float g_vertex_buffer_data[] = {
    0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  0.0f,
    0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  0.0f,
    -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  0.0f,
};

static GLuint program;
static GLuint blocksProgram;
static GLuint u_projection;
static GLuint u_view;
static GLuint vertexbuffer;
static GLuint texture;

bool Program::SetUp()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../imaditor/imgui/extra_fonts/Roboto-Medium.ttf", 16.0f);

    ImFontConfig config;
    config.MergeMode = true;

    static const ImWchar icons_ranges_fontawesome[] = { 0xf000, 0xf3ff, 0 }; // will not be copied by AddFont* so keep
    io.Fonts->AddFontFromFileTTF("../imaditor/fontawesome-webfont.ttf", 18.0f, &config, icons_ranges_fontawesome);

    static const ImWchar icons_ranges_googleicon[] = { 0xe000, 0xeb4c, 0 }; // will not be copied by AddFont* so keep
    io.Fonts->AddFontFromFileTTF("../imaditor/MaterialIcons-Regular.ttf", 18.0f, &config, icons_ranges_googleicon);

    blocksProgram = LoadShaderProgram(vertexBlocksGlsl.c_str(), fragmentBlocksGlsl.c_str());
    program = LoadShaderProgram(vertexGlsl.c_str(), fragmentGlsl.c_str());
    u_projection = glGetUniformLocation(program, "u_projection");
    u_view = glGetUniformLocation(program, "u_view");

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                sizeof(float) * 6,  // stride
                (void*)0            // array buffer offset
                );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
                1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                sizeof(float) * 6,  // stride
                (void*)(sizeof(float) * 3) // array buffer offset
                );

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

static float foreColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static float backColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
static std::vector<std::string> tabs({ "empty" });

#include "image.h"

Layer::Layer() : _flags(0), _visible(true), _alpha(1.0f), _alphaMode(0) { }

Layer* Layer::defaultLayer(int size[2])
{
    auto layer = new Layer();
    layer->_offset[0] = 0;
    layer->_offset[1] = 0;

    layer->_size[0] = size[0];
    layer->_size[1] = size[1];

    layer->_bpp = 4;

    auto dataSize = layer->_size[0] * layer->_size[1] * layer->_bpp;
    layer->_data = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i) layer->_data[i] = 255;

    return layer;
}

Layer* Layer::fromFile(const char* filename)
{
    auto layer = new Layer();

    int n = 4;
    layer->_data = stbi_load(filename, &(layer ->_size[0]), &(layer ->_size[1]), &(layer ->_bpp), 0);

    return layer;
}

Image::Image() : _flags(0), _data(nullptr), _glindex(0), _selectedLayer(0)
{
    _size[0] = _size[1] = 256.0f;
}

Image::~Image() { }

Layer* Image::addLayer()
{
    auto layer = Layer::defaultLayer(this->_size);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size() + 1);
    layer->setDirty();
    this->_layers.push_back(layer);

    return layer;
}

void Image::fromFile(const char* filename)
{
    auto layer = Layer::fromFile(filename);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size());
    layer->setDirty();
    this->_size[0] = layer->_size[0];
    this->_size[1] = layer->_size[1];
    this->_layers.push_back(layer);
}

void Image::selectLayer(int index)
{
    if (index >= 0 && index < this->_layers.size())
        _selectedLayer = index;
}

bool Image::isDirty() const
{
    if (_flags & Dirty) return true;

    for (Layer* layer : this->_layers)
        if (layer->_flags & Dirty) return true;

    return false;
}

void Image::removeCurrentLayer()
{
    if (this->_layers.size() > 1)
    {
        this->_layers.erase(this->_layers.begin() + this->_selectedLayer);
        if (this->_selectedLayer > 0) this->_selectedLayer--;
        this->setDirty();
    }
}

void Image::moveCurrentLayerUp()
{
    if (this->_selectedLayer >= 1)
    {
        auto tmp = this->_layers[this->_selectedLayer];
        this->_layers[this->_selectedLayer] = this->_layers[this->_selectedLayer-1];
        this->_layers[this->_selectedLayer-1] = tmp;
        this->_selectedLayer--;
        this->setDirty();
    }
}

void Image::moveCurrentLayerDown()
{
    if (this->_selectedLayer < this->_layers.size()-1)
    {
        auto tmp = this->_layers[this->_selectedLayer];
        this->_layers[this->_selectedLayer] = this->_layers[this->_selectedLayer+1];
        this->_layers[this->_selectedLayer+1] = tmp;
        this->_selectedLayer++;
        this->setDirty();
    }
}

GLuint uploadImage(Image* img)
{
    auto dataSize = img->_size[0] * img->_size[1] * 4;
    if (img->_data == nullptr) img->_data = new unsigned char[dataSize];

    memset(img->_data, 0, dataSize);

    for (Layer* layer : img->_layers)
    {
        if (!layer->_visible) continue;
        for (int y = 0; y < img->_size[1]; ++y)
        {
            for (int x = 0; x < img->_size[0]; ++x)
            {
                auto p = (x * img->_size[1] + y);
                for (int b = 0; b < layer->_bpp; b++)
                {
                    img->_data[p * 4 + b] += layer->_data[p * layer->_bpp + b];
                }
                if (layer->_bpp < 4) img->_data[p * 4 + 3] = 255;
            }
        }
    }

    if (img->_glindex == 0) glGenTextures(1, &(img->_glindex));
    glBindTexture(GL_TEXTURE_2D, img->_glindex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, img->_size[0], img->_size[1], 0, format, GL_UNSIGNED_BYTE, img->_data);

    return img->_glindex;
}

static int selectedTab = 0;
static Image* selectedImage = nullptr;
static const char** tabNames = nullptr;
static int tabNameCount = 0;
static int tabNameAllocCount = 0;
static std::vector<Image*> _images;

void changeTab(int newTab)
{
    if (selectedImage != nullptr)
    {
        glDeleteTextures(1, &(selectedImage->_glindex));
        selectedImage->_glindex = 0;
        selectedImage = nullptr;
    }

    if (newTab >= 0 && newTab < _images.size())
    {
        selectedImage = _images[newTab];
        uploadImage(selectedImage);
    }
}

void addImage(Image* img)
{
    _images.push_back(img);

    if (tabNames != nullptr) delete []tabNames;
    if (tabNameCount >= tabNameAllocCount)
    {
        tabNameAllocCount += 32;
        auto tmp = new const char*[tabNameAllocCount];
        for (int i = 0; i < tabNameCount; i++)
        {
            tmp[i] = new char[strlen(tabNames[i]) + 1];
            strcpy(((char*)tmp[i]), tabNames[i]);
            delete [](tabNames[i]);
        }
        delete []tabNames;
        tabNames = tmp;
    }
    tabNames[tabNameCount] = new char[strlen(img->_name.c_str()) + 1];
    strcpy(((char*)tabNames[tabNameCount]), img->_name.c_str());
    selectedTab = tabNameCount;
    ++tabNameCount;
    changeTab(selectedTab);
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
    if (selectedImage != nullptr)
    {
        selectedImage->addLayer();
    }
}

void removeCurrentLayer()
{
    if (selectedImage != nullptr)
    {
        selectedImage->removeCurrentLayer();
    }
}

void moveCurrentLayerUp()
{
    if (selectedImage != nullptr)
    {
        selectedImage->moveCurrentLayerUp();
    }
}

void moveCurrentLayerDown()
{
    if (selectedImage != nullptr)
    {
        selectedImage->moveCurrentLayerDown();
    }
}

void Program::Render()
{
    glViewport(0, 0, this->_display_w, this->_display_h);
    glClearColor(114/255.0f, 144/255.0f, 154/255.0f, 255/255.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (selectedImage != nullptr)
    {
        if (selectedImage->isDirty()) uploadImage(selectedImage);

        auto zoom = glm::scale(glm::mat4(), glm::vec3(windowConfig.zoom / 100.0f));
        auto translate = glm::translate(zoom, glm::vec3(windowConfig.translatex, windowConfig.translatey, 0.0f));
        auto projection = glm::ortho(-(_display_w/2.0f), (_display_w/2.0f),
                                     (_display_h/2.0f), -(_display_h/2.0f));

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        auto full = glm::scale(glm::mat4(), glm::vec3(selectedImage->_size[0], selectedImage->_size[1], 1.0f));
        glUseProgram(blocksProgram);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, &((projection * translate * full)[0][0]));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(program);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, &((projection * translate)[0][0]));

        glBindTexture(GL_TEXTURE_2D, selectedImage->_glindex);
        auto view = glm::scale(glm::mat4(), glm::vec3(selectedImage->_size[0], selectedImage->_size[1], 1.0f));
        glUniformMatrix4fv(u_view, 1, GL_FALSE, &(view[0][0]));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "CTRL+N")) newImage();
                    if (ImGui::MenuItem("Open", "CTRL+O")) openImage();
                    if (ImGui::MenuItem("Save", "CTRL+S")) {}
                    if (ImGui::MenuItem("Save As..", "CTRL+SHIFT+Z")) {}
                    if (ImGui::MenuItem("Close")) {}
                    ImGui::Separator();
                    if (ImGui::MenuItem("Quit")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                    ImGui::Separator();
                    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About IMaditor")) {}
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }
        ImGui::PopStyleColor();

        const int dockbarWidth = 250;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.20f, 0.20f, 0.47f, 0.60f));
        {
            ImGui::Begin("toolbar", &windowConfig.show_toolbar, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |  ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(0, 22));
                ImGui::SetWindowSize(ImVec2(45, this->_display_h - 57));

                for (int i = 0; i < tools.toolCount(); i++)
                {
                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_Button, i == tools.selectedToolIndex() ? ImVec4(1.0f, 1.0f, 1.0f, 0.0f) : ImGui::GetStyle().Colors[ImGuiCol_Button]);
                    if (ImGui::Button(tools[i]._icon, ImVec2(30, 30))) tools.selectTool(i);
                    ImGui::PopStyleColor(1);
                    ImGui::PopID();
                }
            }
            ImGui::End();

            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
            ImGui::Begin("content", &(windowConfig.show_listeditor), ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(45, 22));
                ImGui::SetWindowSize(ImVec2(this->_display_w - 45 - dockbarWidth, this->_display_h - 57));

                if (tabNameCount > 0)
                {
                    if (ImGui::TabLabels(tabNames, tabNameCount, selectedTab))
                    {
                        changeTab(selectedTab);
                    }
                }
            }
            ImGui::End();
            ImGui::PopStyleColor();

            ImGui::Begin("dockbar", &(windowConfig.show_browser), ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(this->_display_w - dockbarWidth, 22));
                ImGui::SetWindowSize(ImVec2(dockbarWidth, this->_display_h - 57));

                if (ImGui::CollapsingHeader("Color options", "colors", true, true))
                {
                    ImGui::ColorEdit3("Fore", foreColor);
                    ImGui::ColorEdit3("Back", backColor);
                }

                if (_images.size() > 0)
                {
                    if (ImGui::CollapsingHeader("Layer options", "layers", true, true))
                    {
                        if (ImGui::Button(FontAwesomeIcons::FA_PLUS, ImVec2(30.0f, 30.0f))) addLayer();
                        ImGui::SameLine();
                        if (ImGui::Button(FontAwesomeIcons::FA_MINUS, ImVec2(30.0f, 30.0f))) removeCurrentLayer();
                        ImGui::SameLine();
                        if (ImGui::Button(FontAwesomeIcons::FA_ARROW_UP, ImVec2(30.0f, 30.0f))) moveCurrentLayerUp();
                        ImGui::SameLine();
                        if (ImGui::Button(FontAwesomeIcons::FA_ARROW_DOWN, ImVec2(30.0f, 30.0f))) moveCurrentLayerDown();

                        ImGui::Separator();

                        if (selectedImage != nullptr)
                        {
                            for (int i = 0; i < selectedImage->_layers.size(); i++)
                            {
                                auto layer = selectedImage->_layers[i];
                                ImGui::PushID(i);
                                auto title = layer->_name;
                                if (selectedImage->_selectedLayer == i) title += " (selected)";
                                if (ImGui::TreeNode("layer_node", title.c_str()))
                                {
                                    if (ImGui::Button(layer->isVisible() ? FontAwesomeIcons::FA_EYE : FontAwesomeIcons::FA_EYE_SLASH, ImVec2(30, 30)))
                                    {
                                        layer->toggleVisibility();
                                    }
                                    ImGui::SameLine();
                                    ImGui::PushStyleColor(ImGuiCol_Button, i == selectedImage->_selectedLayer ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.20f, 0.40f, 0.47f, 0.0f));
                                    if (ImGui::Button(layer->_name.c_str(), ImVec2(-1, 30))) selectedImage->selectLayer(i);
                                    ImGui::PopStyleColor(1);

                                    ImGui::SliderFloat("Alpha", &(layer->_alpha), 0.0f, 1.0f);
                                    ImGui::Combo("Mode", &(layer->_alphaMode), "Normal\0Darken\0Lighten\0Hue\0Saturation\0Color\0Lumminance\0Multiply\0Screen\0Dissolve\0Overlay\0Hard Light\0Soft Light\0Difference\0Dodge\0Burn\0Exclusion\0\0");

                                    ImGui::TreePop();
                                }
                                ImGui::PopID();
                            }
                        }

                        ImGui::Separator();
                    }
                }

                if (ImGui::CollapsingHeader("Tool options", "tools", true, true))
                {
                    ImGui::TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
                }
            }
            ImGui::End();

            ImGui::Begin("statusbar", &(windowConfig.show_listeditor), ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
            {
                ImGui::SetWindowPos(ImVec2(0, this->_display_h - 35));
                ImGui::SetWindowSize(ImVec2(this->_display_w, 35));

                ImGui::Columns(3);
                ImGui::Text("status bar");
                ImGui::NextColumn();
                ImGui::SliderInt("zoom", &(windowConfig.zoom), 10, 400);
                ImGui::NextColumn();
                ImGui::Text("mouse: %d %d", windowConfig.mousex, windowConfig.mousey);
                ImGui::Columns(1);
            }
            ImGui::End();
        }
        ImGui::PopStyleColor();
    }
    ImGui::PopStyleVar();

    ImGui::Render();
}

void Program::onKeyAction(int key, int scancode, int action, int mods)
{
    windowConfig.shiftPressed = (mods & GLFW_MOD_SHIFT);
    windowConfig.ctrlPressed = (mods & GLFW_MOD_CONTROL);
}

void Program::onMouseMove(int x, int y)
{
    windowConfig.mousex = x;
    windowConfig.mousey = y;
}

void Program::onMouseButton(int button, int action, int mods)
{
    if (selectedImage == nullptr) return;

    if (tools.selectedTool()._actionFactory == nullptr) return;

    auto fac = tools.selectedTool()._actionFactory;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        fac->PrimaryMouseButtonDown(selectedImage,
                                    mods & GLFW_MOD_SHIFT,
                                    mods & GLFW_MOD_CONTROL,
                                    mods & GLFW_MOD_ALT,
                                    mods & GLFW_MOD_SUPER);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        fac->PrimaryMouseButtonUp(selectedImage,
                                  mods & GLFW_MOD_SHIFT,
                                  mods & GLFW_MOD_CONTROL,
                                  mods & GLFW_MOD_ALT,
                                  mods & GLFW_MOD_SUPER);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        fac->PrimaryMouseButtonUp(selectedImage,
                                  mods & GLFW_MOD_SHIFT,
                                  mods & GLFW_MOD_CONTROL,
                                  mods & GLFW_MOD_ALT,
                                  mods & GLFW_MOD_SUPER);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        fac->PrimaryMouseButtonUp(selectedImage,
                                  mods & GLFW_MOD_SHIFT,
                                  mods & GLFW_MOD_CONTROL,
                                  mods & GLFW_MOD_ALT,
                                  mods & GLFW_MOD_SUPER);
    }
}

void Program::onScroll(int x, int y)
{
    if (windowConfig.shiftPressed)
    {
        windowConfig.translatex += (y * 5);
    }
    else if (windowConfig.ctrlPressed)
    {
        windowConfig.translatey += (y * 5);
    }
    else
    {
        windowConfig.zoom += (y * 5);
        if (windowConfig.zoom < 10) windowConfig.zoom = 10;
    }
}

void Program::onResize(int width, int height)
{
    float aspectw = float(width) / float(this->_display_w);
    float aspecth = float(height) / float(this->_display_h);
    windowConfig.w *= aspectw;
    windowConfig.h *= aspecth;

    this->_display_w = width;
    this->_display_h = height;

    glViewport(0, 0, width, height);
}

void Program::CleanUp()
{ }

void Program::KeyActionCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onKeyAction(key, scancode, action, mods);
}

void Program::CursorPosCallback(GLFWwindow* window, double x, double y)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onMouseMove(int(x), int(y));
}

void Program::ScrollCallback(GLFWwindow* window, double x, double y)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onScroll(int(x), int(y));
}

void Program::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onMouseButton(button, action, mods);
}

void Program::ResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onResize(width, height);
}
