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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

void Program::ResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onResize(width, height);
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

static struct Tool { const char* name; const char* icon; } tools[] = {
{ "Arrow", FontAwesomeIcons::FA_MOUSE_POINTER },
{ "Zoom", FontAwesomeIcons::FA_SEARCH },
{ "Deformation", FontAwesomeIcons::FA_QUESTION },
{ "Crop", FontAwesomeIcons::FA_CROP },
{ "Mover", FontAwesomeIcons::FA_ARROWS },
{ "Selection", FontAwesomeIcons::FA_QUESTION },
{ "Freehand", FontAwesomeIcons::FA_QUESTION },
{ "Magic wand selection", FontAwesomeIcons::FA_MAGIC },
{ "Dropper", FontAwesomeIcons::FA_EYEDROPPER },
{ "Paint brush", FontAwesomeIcons::FA_PAINT_BRUSH },
{ "Clone brush", FontAwesomeIcons::FA_QUESTION },
{ "Color replacer", FontAwesomeIcons::FA_CLONE },
{ "Retouch brush", FontAwesomeIcons::FA_HAND_O_DOWN },
{ "Scratch remover", FontAwesomeIcons::FA_QUESTION },
{ "Erase", FontAwesomeIcons::FA_ERASER },
{ "Picture tube", FontAwesomeIcons::FA_QUESTION },
{ "Airbrush", FontAwesomeIcons::FA_QUESTION },
{ "Flood fill", FontAwesomeIcons::FA_TINT },
{ "Text", FontAwesomeIcons::FA_FONT },
{ "Draw", GoogleIcons::GI_GESTURE},
{ "Preset shapes", FontAwesomeIcons::FA_QUESTION },
{ "Vector object selector", FontAwesomeIcons::FA_QUESTION }
};

static int selectedTool = 0;
static float foreColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static float backColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
static std::vector<std::string> tabs({ "empty" });

void selectTool(int index)
{
    selectedTool = index;
    std::cout << "Selecting tool: " << index << std::endl;
}

class Layer
{
public:
    Layer();

    std::string _name;
    bool _visible;
    float _alpha;
    int _alphaMode;

    int _offset[2];
    int _size[2];
    int _bpp;
    unsigned char* _data;
    GLuint glindex;

    void upload();
    void use();

    static Layer* defaultLayer(int size[2]);
    static Layer* fromFile(const char* filename);
};

Layer::Layer() : _visible(true), _alpha(1.0f), _alphaMode(0), glindex(0) { }

void Layer::upload()
{
    glGenTextures(1, &glindex);
    glBindTexture(GL_TEXTURE_2D, glindex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint format = GL_RGBA;
    if (this->_bpp == 3) format = GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, this->_size[0], this->_size[1], 0, format, GL_UNSIGNED_BYTE, this->_data);
}

void Layer::use()
{
    glBindTexture(GL_TEXTURE_2D, glindex);
}

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

    layer->upload();

    return layer;
}

Layer* Layer::fromFile(const char* filename)
{
    auto layer = new Layer();

    int n = 4;
    layer->_data = stbi_load(filename, &(layer ->_size[0]), &(layer ->_size[1]), &(layer ->_bpp), 0);

    layer->upload();

    return layer;
}

class Document
{
public:
    Document();
    Document(const char* name);
    virtual ~Document();

    std::vector<Layer*> _layers;
    std::string _name;
    std::string _fullPath;
    int _size[2];

    Layer* addLayer();
    void fromFile(const char* filename);
};

Document::Document() { _size[0] = _size[1] = 256.0f; }

Document::~Document() { }

Layer* Document::addLayer()
{
    auto layer = Layer::defaultLayer(this->_size);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size());
    this->_layers.push_back(layer);

    return layer;
}

void Document::fromFile(const char* filename)
{
    auto layer = Layer::fromFile(filename);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size());
    this->_size[0] = layer->_size[0];
    this->_size[1] = layer->_size[1];
    this->_layers.push_back(layer);
}

static int selectedTab = 0;
static const char** tabNames = nullptr;
static int tabNameCount = 0;
static int tabNameAllocCount = 0;
static std::vector<Document*> _documents;
static int selectedLayer = 0;

void addDocument(Document* doc)
{
    _documents.push_back(doc);

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
    tabNames[tabNameCount] = new char[strlen(doc->_name.c_str()) + 1];
    strcpy(((char*)tabNames[tabNameCount]), doc->_name.c_str());
    ++tabNameCount;
}

void newDocument()
{
    auto doc = new Document();
    doc->_name = "New";
    doc->_fullPath = "New.png";
    doc->addLayer();
    addDocument(doc);
}

void openDocument()
{
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

    if (result == NFD_OKAY)
    {
        auto doc = new Document();
        doc->_fullPath = outPath;
        std::replace(doc->_fullPath.begin(), doc->_fullPath.end(), '\\', '/');
        doc->_name = doc->_fullPath.substr(doc->_fullPath.find_last_of('/') + 1);
        doc->fromFile(outPath);
        addDocument(doc);
    }
}

void addLayer()
{
    if (_documents.size() > 0)
    {
        Document* doc = _documents[selectedTab];
        doc->addLayer();
    }
}

void removeCurrentLayer()
{
    if (_documents.size() > 0)
    {
        Document* doc = _documents[selectedTab];
        if (doc->_layers.size() > 1)
        {
            doc->_layers.erase(doc->_layers.begin() + selectedLayer);
            if (selectedLayer > 0) selectedLayer--;
        }
    }
}

void moveCurrentLayerUp()
{
    if (_documents.size() > 0)
    {
        Document* doc = _documents[selectedTab];
        if (selectedLayer >= 1)
        {
            auto tmp = doc->_layers[selectedLayer];
            doc->_layers[selectedLayer] = doc->_layers[selectedLayer-1];
            doc->_layers[selectedLayer-1] = tmp;
            selectedLayer--;
        }
    }
}

void moveCurrentLayerDown()
{
    if (_documents.size() > 0)
    {
        Document* doc = _documents[selectedTab];
        if (selectedLayer < doc->_layers.size()-1)
        {
            auto tmp = doc->_layers[selectedLayer];
            doc->_layers[selectedLayer] = doc->_layers[selectedLayer+1];
            doc->_layers[selectedLayer+1] = tmp;
            selectedLayer++;
        }
    }
}

void selectLayer(int index)
{
    selectedLayer = index;
}

void Program::Render()
{
    glViewport(0, 0, this->_display_w, this->_display_h);
    glClearColor(114/255.0f, 144/255.0f, 154/255.0f, 255/255.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (selectedTab < _documents.size())
    {
        Document* doc = _documents[selectedTab];

        auto zoom = glm::scale(glm::mat4(), glm::vec3(windowConfig.zoom / 100.0f));
        auto translate = glm::translate(zoom, glm::vec3(windowConfig.translatex, windowConfig.translatey, 0.0f));
        auto projection = glm::ortho(-(_display_w/2.0f), (_display_w/2.0f),
                                     (_display_h/2.0f), -(_display_h/2.0f));

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // render blocks on document background
        auto full = glm::scale(glm::mat4(), glm::vec3(doc->_size[0], doc->_size[1], 1.0f));
        glUseProgram(blocksProgram);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, &((projection * translate * full)[0][0]));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // render all layers
        glUseProgram(program);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, &((projection * translate)[0][0]));

        for (Layer* layer : doc->_layers)
        {
            if (!layer->_visible) continue;
            layer->use();

            auto view = glm::scale(glm::mat4(), glm::vec3(layer->_size[0], layer->_size[1], 1.0f));
            glUniformMatrix4fv(u_view, 1, GL_FALSE, &(view[0][0]));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
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
                    if (ImGui::MenuItem("New", "CTRL+N")) newDocument();
                    if (ImGui::MenuItem("Open", "CTRL+O")) openDocument();
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

                for (int i = 0; i < sizeof(tools) / sizeof(Tool); i++)
                {
                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_Button, i == selectedTool ? ImVec4(1.0f, 1.0f, 1.0f, 0.0f) : ImGui::GetStyle().Colors[ImGuiCol_Button]);
                    if (ImGui::Button(tools[i].icon, ImVec2(30, 30))) selectTool(i);
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
                    ImGui::TabLabels(tabNames, tabNameCount, selectedTab);
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

                if (_documents.size() > 0)
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

                        Document* doc = _documents[selectedTab];
                        for (int i = 0; i < doc->_layers.size(); i++)
                        {
                            auto layer = doc->_layers[i];
                            ImGui::PushID(i);
                            auto title = layer->_name;
                            if (selectedLayer == i) title += " (selected)";
                            if (ImGui::TreeNode("layer_node", title.c_str()))
                            {
                                if (ImGui::Button(layer->_visible ? FontAwesomeIcons::FA_EYE : FontAwesomeIcons::FA_EYE_SLASH, ImVec2(30, 30)))
                                    layer->_visible = !layer->_visible;
                                ImGui::SameLine();
                                ImGui::PushStyleColor(ImGuiCol_Button, i == selectedLayer ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImVec4(0.20f, 0.40f, 0.47f, 0.0f));
                                if (ImGui::Button(layer->_name.c_str(), ImVec2(-1, 30))) selectLayer(i);
                                ImGui::PopStyleColor(1);

                                ImGui::SliderFloat("Alpha", &(layer->_alpha), 0.0f, 1.0f);
                                ImGui::Combo("Mode", &(layer->_alphaMode), "Normal\0Darken\0Lighten\0Hue\0Saturation\0Color\0Lumminance\0Multiply\0Screen\0Dissolve\0Overlay\0Hard Light\0Soft Light\0Difference\0Dodge\0Burn\0Exclusion\0\0");

                                ImGui::TreePop();
                            }
                            ImGui::PopID();
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

void Program::CleanUp()
{ }
