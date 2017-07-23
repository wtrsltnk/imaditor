#include "program.h"
#include <GL/glextl.h>
#include <nfd.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_impl_glfw_gl3.h"
#include "font-icons.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ImGui {
/*
    tabLabels: name of all tabs involved
    tabSize: number of elements
    tabIndex: holds the current active tab
    tabOrder: optional array of integers from 0 to tabSize-1 that maps the tab label order. If one of the numbers is replaced by -1 the tab label is not visible (closed). It can be read/modified at runtime.

    // USAGE EXAMPLE
    static const char* tabNames[] = {"First tab","Second tab","Third tab"};
    static int tabOrder[] = {0,1,2};
    static int tabSelected = 0;
    const bool tabChanged = ImGui::TabLabels(tabNames,sizeof(tabNames)/sizeof(tabNames[0]),tabSelected,tabOrder);
    ImGui::Text("\nTab Page For Tab: \"%s\" here.\n",tabNames[tabSelected]);
*/

IMGUI_API bool TabLabels(const char **tabLabels, int tabSize, int &tabIndex, int *tabOrder=NULL) {
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec2 itemSpacing =  style.ItemSpacing;
    const ImVec4 color =        style.Colors[ImGuiCol_Button];
    const ImVec4 colorActive =  style.Colors[ImGuiCol_ButtonActive];
    const ImVec4 colorHover =   style.Colors[ImGuiCol_ButtonHovered];
    const ImVec4 colorText =    style.Colors[ImGuiCol_Text];
    style.ItemSpacing.x =       1;
    style.ItemSpacing.y =       1;
    const ImVec4 colorSelectedTab = ImVec4(color.x,color.y,color.z,color.w*0.5f);
    const ImVec4 colorSelectedTabHovered = ImVec4(colorHover.x,colorHover.y,colorHover.z,colorHover.w*0.5f);
    const ImVec4 colorSelectedTabText = ImVec4(colorText.x*0.8f,colorText.y*0.8f,colorText.z*0.8f,colorText.w*0.8f);

    if (tabSize>0 && (tabIndex<0 || tabIndex>=tabSize)) {
        if (!tabOrder)  tabIndex = 0;
        else tabIndex = -1;
    }

    float windowWidth = 0.f,sumX=0.f;
    windowWidth = ImGui::GetWindowWidth() - style.WindowPadding.x - (ImGui::GetScrollMaxY()>0 ? style.ScrollbarSize : 0.f);

    static int draggingTabIndex = -1;int draggingTabTargetIndex = -1;   // These are indices inside tabOrder
    static ImVec2 draggingtabSize(0,0);
    static ImVec2 draggingTabOffset(0,0);

    const bool isMMBreleased = ImGui::IsMouseReleased(2);
    const bool isMouseDragging = ImGui::IsMouseDragging(0,2.f);
    int justClosedTabIndex = -1,newtabIndex = tabIndex;


    bool selection_changed = false;bool noButtonDrawn = true;
    for (int j = 0,i; j < tabSize; j++)
    {
        i = tabOrder ? tabOrder[j] : j;
        if (i==-1) continue;

        if (sumX > 0.f) {
            sumX+=style.ItemSpacing.x;   // Maybe we can skip it if we use SameLine(0,0) below
            sumX+=ImGui::CalcTextSize(tabLabels[i]).x+2.f*style.FramePadding.x;
            if (sumX>windowWidth) sumX = 0.f;
            else ImGui::SameLine();
        }

        if (i != tabIndex) {
            // Push the style
            style.Colors[ImGuiCol_Button] =         colorSelectedTab;
            style.Colors[ImGuiCol_ButtonActive] =   colorSelectedTab;
            style.Colors[ImGuiCol_ButtonHovered] =  colorSelectedTabHovered;
            style.Colors[ImGuiCol_Text] =           colorSelectedTabText;
        }
        // Draw the button
        ImGui::PushID(i);   // otherwise two tabs with the same name would clash.
        if (ImGui::Button(tabLabels[i]))   {selection_changed = (tabIndex!=i);newtabIndex = i;}
        ImGui::PopID();
        if (i != tabIndex) {
            // Reset the style
            style.Colors[ImGuiCol_Button] =         color;
            style.Colors[ImGuiCol_ButtonActive] =   colorActive;
            style.Colors[ImGuiCol_ButtonHovered] =  colorHover;
            style.Colors[ImGuiCol_Text] =           colorText;
        }
        noButtonDrawn = false;

        if (sumX==0.f) sumX = style.WindowPadding.x + ImGui::GetItemRectSize().x; // First element of a line

        if (ImGui::IsItemHoveredRect()) {
            if (tabOrder)  {
                // tab reordering
                if (isMouseDragging) {
                    if (draggingTabIndex==-1) {
                        draggingTabIndex = j;
                        draggingtabSize = ImGui::GetItemRectSize();
                        const ImVec2& mp = ImGui::GetIO().MousePos;
                        const ImVec2 draggingTabCursorPos = ImGui::GetCursorPos();
                        draggingTabOffset=ImVec2(
                                    mp.x+draggingtabSize.x*0.5f-sumX+ImGui::GetScrollX(),
                                    mp.y+draggingtabSize.y*0.5f-draggingTabCursorPos.y+ImGui::GetScrollY()
                                    );

                    }
                }
                else if (draggingTabIndex>=0 && draggingTabIndex<tabSize && draggingTabIndex!=j){
                    draggingTabTargetIndex = j; // For some odd reasons this seems to get called only when draggingTabIndex < i ! (Probably during mouse dragging ImGui owns the mouse someway and sometimes ImGui::IsItemHovered() is not getting called)
                }
            }
        }

    }

    tabIndex = newtabIndex;

    // Draw tab label while mouse drags it
    if (draggingTabIndex>=0 && draggingTabIndex<tabSize) {
        const ImVec2& mp = ImGui::GetIO().MousePos;
        const ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 start(wp.x+mp.x-draggingTabOffset.x-draggingtabSize.x*0.5f,wp.y+mp.y-draggingTabOffset.y-draggingtabSize.y*0.5f);
        const ImVec2 end(start.x+draggingtabSize.x,start.y+draggingtabSize.y);
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const float draggedBtnAlpha = 0.65f;
        const ImVec4& btnColor = style.Colors[ImGuiCol_Button];
        drawList->AddRectFilled(start,end,ImColor(btnColor.x,btnColor.y,btnColor.z,btnColor.w*draggedBtnAlpha),style.FrameRounding);
        start.x+=style.FramePadding.x;start.y+=style.FramePadding.y;
        const ImVec4& txtColor = style.Colors[ImGuiCol_Text];
        drawList->AddText(start,ImColor(txtColor.x,txtColor.y,txtColor.z,txtColor.w*draggedBtnAlpha),tabLabels[tabOrder[draggingTabIndex]]);

        ImGui::SetMouseCursor(ImGuiMouseCursor_Move);
    }

    // Drop tab label
    if (draggingTabTargetIndex!=-1) {
        // swap draggingTabIndex and draggingTabTargetIndex in tabOrder
        const int tmp = tabOrder[draggingTabTargetIndex];
        tabOrder[draggingTabTargetIndex] = tabOrder[draggingTabIndex];
        tabOrder[draggingTabIndex] = tmp;
        //fprintf(stderr,"%d %d\n",draggingTabIndex,draggingTabTargetIndex);
        draggingTabTargetIndex = draggingTabIndex = -1;
    }

    // Reset draggingTabIndex if necessary
    if (!isMouseDragging) draggingTabIndex = -1;

    // Change selected tab when user closes the selected tab
    if (tabIndex == justClosedTabIndex && tabIndex>=0)    {
        tabIndex = -1;
        for (int j = 0,i; j < tabSize; j++) {
            i = tabOrder ? tabOrder[j] : j;
            if (i==-1) continue;
            tabIndex = i;
            break;
        }
    }

    // Restore the style
    style.Colors[ImGuiCol_Button] =         color;
    style.Colors[ImGuiCol_ButtonActive] =   colorActive;
    style.Colors[ImGuiCol_ButtonHovered] =  colorHover;
    style.Colors[ImGuiCol_Text] =           colorText;
    style.ItemSpacing =                     itemSpacing;

    return selection_changed;
}
} // namespace ImGui

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

void Program::ResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = static_cast<Program*>(glfwGetWindowUserPointer(window));

    if (app != nullptr) app->onResize(width, height);
}

void Program::onKeyAction(int key, int scancode, int action, int mods)
{ }

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

static float g_vertex_buffer_data[] = {
    0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  0.0f,
    0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  0.0f,
    -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  0.0f,
};

static GLuint program;
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

const ImVec4 clear_color = ImColor(114, 144, 154);

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

    int _offset[2];
    int _size[2];
    int _bpp;
    unsigned char* _data;
    GLuint glindex;

    void upload();
    void use();

    static Layer* defaultLayer();
    static Layer* fromFile(const char* filename);
};

Layer::Layer() : glindex(0) { }

void Layer::upload()
{
    glGenTextures(1, &glindex);
    glBindTexture(GL_TEXTURE_2D, glindex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLint format = GL_RGBA;
    if (this->_bpp == 3) format = GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, this->_size[0], this->_size[1], 0, format, GL_UNSIGNED_BYTE, this->_data);
}

void Layer::use()
{
    glBindTexture(GL_TEXTURE_2D, glindex);
}

Layer* Layer::defaultLayer()
{
    auto layer = new Layer();
    layer->_offset[0] = 0;
    layer->_offset[1] = 0;

    layer->_size[0] = 128;
    layer->_size[1] = 128;

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

    std::string _name;
    std::vector<Layer*> _layers;

    void fromFile(const char* filename);
};

Document::Document() { }

Document::~Document() { }

static int selectedTab = 0;
static const char** tabNames = nullptr;
static int tabNameCount = 0;
static int tabNameAllocCount = 0;
static std::vector<Document*> _documents;

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

void Program::Render()
{
    glViewport(0, 0, this->_display_w, this->_display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    auto projection = glm::ortho(
                -(this->_display_w/2.0f),
                (this->_display_w/2.0f),
                (this->_display_h/2.0f),
                -(this->_display_h/2.0f));
    glUseProgram(program);
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, &(projection[0][0]));

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (selectedTab < _documents.size())
    {
        Document* doc = _documents[selectedTab];
        for (Layer* layer : doc->_layers)
        {
            layer->use();

            auto view = glm::scale(glm::mat4(), glm::vec3(layer->_size[0], layer->_size[1], 1.0f));
            glUniformMatrix4fv(u_view, 1, GL_FALSE, &(view[0][0]));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    ImGui_ImplGlfwGL3_NewFrame();

    const ImVec4 activeButtonColor = ImVec4(0.20f, 0.20f, 0.57f, 0.60f);
    const ImVec4 inactiveButtonColor = ImVec4(0.20f, 0.40f, 0.47f, 0.60f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
    {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "CTRL+N"))
                {
                    auto doc = new Document();
                    doc->_name = "New";
                    doc->_layers.push_back(Layer::defaultLayer());
                    addDocument(doc);
                }
                if (ImGui::MenuItem("Open", "CTRL+O"))
                {
                    nfdchar_t *outPath = NULL;
                    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

                    if (result == NFD_OKAY)
                    {
                        auto doc = new Document();
                        doc->_name = outPath;
                        doc->_layers.push_back(Layer::fromFile(outPath));
                        addDocument(doc);
                    }
                }
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
            ImGui::SetWindowSize(ImVec2(45, this->_display_h - 22));

            for (int i = 0; i < sizeof(tools) / sizeof(Tool); i++)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, i == selectedTool ? inactiveButtonColor : activeButtonColor);
                if (ImGui::Button(tools[i].icon, ImVec2(30, 30))) selectTool(i);
                ImGui::PopStyleColor(1);
            }
        }
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
        ImGui::Begin("content", &(windowConfig.show_listeditor), ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::SetWindowPos(ImVec2(45, 22));
            ImGui::SetWindowSize(ImVec2(this->_display_w - 45 - dockbarWidth, this->_display_h - 22));

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
            ImGui::SetWindowSize(ImVec2(dockbarWidth, this->_display_h - 22));

            if (ImGui::CollapsingHeader("Color options", "colors", true, true))
            {
                ImGui::ColorEdit3("Fore", foreColor);
                ImGui::ColorEdit3("Back", backColor);

                ImGui::TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
            }

            if (ImGui::CollapsingHeader("Layer options", "layers", true, true))
            {
                ImGui::TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
            }

            if (ImGui::CollapsingHeader("Tool options", "tools", true, true))
            {
                ImGui::TextWrapped("This window is being created by the ShowTestWindow() function. Please refer to the code for programming reference.\n\nUser Guide:");
            }
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
