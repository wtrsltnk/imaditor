#include "imgui_tabs.h"

namespace ImGui {

IMGUI_API bool TabLabels(const std::vector<std::string> &tabLabels, int tabSize, int &tabIndex, int *tabOrder) {
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
            sumX+=ImGui::CalcTextSize(tabLabels[i].c_str()).x+2.f*style.FramePadding.x;
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
        if (ImGui::Button(tabLabels[i].c_str()))   {selection_changed = (tabIndex!=i);newtabIndex = i;}
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

        if (ImGui::IsItemHovered()) {
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
        drawList->AddText(start,ImColor(txtColor.x,txtColor.y,txtColor.z,txtColor.w*draggedBtnAlpha),tabLabels[tabOrder[draggingTabIndex]].c_str());

        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
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

bool ColorPicker(const char* label, float col[3])
{
    static const float HUE_PICKER_WIDTH = 20.0f;
    static const float CROSSHAIR_SIZE = 7.0f;
    static const ImVec2 SV_PICKER_SIZE = ImVec2(200, 200);

    ImColor color(col[0], col[1], col[2]);
    bool value_changed = false;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 picker_pos = ImGui::GetCursorScreenPos();

    ImColor colors[] = { ImColor(255, 0, 0),
        ImColor(255, 255, 0),
        ImColor(0, 255, 0),
        ImColor(0, 255, 255),
        ImColor(0, 0, 255),
        ImColor(255, 0, 255),
        ImColor(255, 0, 0) };

    for (int i = 0; i < 6; ++i)
    {
        draw_list->AddRectFilledMultiColor(
            ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
            ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10 + HUE_PICKER_WIDTH,
            picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
            colors[i],
            colors[i],
            colors[i + 1],
            colors[i + 1]);
    }

    float hue, saturation, value;
    ImGui::ColorConvertRGBtoHSV(
        color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

    draw_list->AddLine(
        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 8, picker_pos.y + hue * SV_PICKER_SIZE.y),
        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 12 + HUE_PICKER_WIDTH, picker_pos.y + hue * SV_PICKER_SIZE.y),
        ImColor(255, 255, 255));

    {
        const int step = 5;
        ImVec2 pos = ImVec2(0, 0);

        ImVec4 c00(1, 1, 1, 1);
        ImVec4 c10(1, 1, 1, 1);
        ImVec4 c01(1, 1, 1, 1);
        ImVec4 c11(1, 1, 1, 1);
        for (int y = 0; y < step; y++) {
            for (int x = 0; x < step; x++) {
                float s0 = (float)x / (float)step;
                float s1 = (float)(x + 1) / (float)step;
                float v0 = 1.0 - (float)(y) / (float)step;
                float v1 = 1.0 - (float)(y + 1) / (float)step;

                ImGui::ColorConvertHSVtoRGB(hue, s0, v0, c00.x, c00.y, c00.z);
                ImGui::ColorConvertHSVtoRGB(hue, s1, v0, c10.x, c10.y, c10.z);
                ImGui::ColorConvertHSVtoRGB(hue, s0, v1, c01.x, c01.y, c01.z);
                ImGui::ColorConvertHSVtoRGB(hue, s1, v1, c11.x, c11.y, c11.z);

                draw_list->AddRectFilledMultiColor(
                    ImVec2(picker_pos.x + pos.x, picker_pos.y + pos.y),
                    ImVec2(picker_pos.x + pos.x + SV_PICKER_SIZE.x / step, picker_pos.y + pos.y + SV_PICKER_SIZE.y / step),
                    ImGui::ColorConvertFloat4ToU32(c00),
                    ImGui::ColorConvertFloat4ToU32(c10),
                    ImGui::ColorConvertFloat4ToU32(c11),
                    ImGui::ColorConvertFloat4ToU32(c01));

                pos.x += SV_PICKER_SIZE.x / step;
            }
            pos.x = 0;
            pos.y += SV_PICKER_SIZE.y / step;
        }
    }

    float x = saturation * SV_PICKER_SIZE.x;
    float y = (1 -value) * SV_PICKER_SIZE.y;
    ImVec2 p(picker_pos.x + x, picker_pos.y + y);
    draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
    draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
    draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
    draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

    ImGui::InvisibleButton("saturation_value_selector", SV_PICKER_SIZE);

    if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
    {
        ImVec2 mouse_pos_in_canvas = ImVec2(
            ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

        /**/ if( mouse_pos_in_canvas.x <                     0 ) mouse_pos_in_canvas.x = 0;
        else if( mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1 ) mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

        /**/ if( mouse_pos_in_canvas.y <                     0 ) mouse_pos_in_canvas.y = 0;
        else if( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1 ) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

        value = 1 - (mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1));
        saturation = mouse_pos_in_canvas.x / (SV_PICKER_SIZE.x - 1);
        value_changed = true;
    }

    ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10, picker_pos.y));
    ImGui::InvisibleButton("hue_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

    if( (ImGui::IsItemHovered()||ImGui::IsItemActive()) && ImGui::GetIO().MouseDown[0])
    {
        ImVec2 mouse_pos_in_canvas = ImVec2(
            ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

        /* Previous horizontal bar will represent hue=1 (bottom) as hue=0 (top). Since both colors are red, we clamp at (-2, above edge) to avoid visual continuities */
        /**/ if( mouse_pos_in_canvas.y <                     0 ) mouse_pos_in_canvas.y = 0;
        else if( mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 2 ) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 2;

        hue = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1 );
        value_changed = true;
    }

    color = ImColor::HSV(hue > 0 ? hue : 1e-6, saturation > 0 ? saturation : 1e-6, value > 0 ? value : 1e-6);
    col[0] = color.Value.x;
    col[1] = color.Value.y;
    col[2] = color.Value.z;
    return value_changed | ImGui::ColorEdit3(label, col);
}

} // namespace ImGui
