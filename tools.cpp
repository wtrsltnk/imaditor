#include "tools.h"

#include <IconsFontAwesome4.h>
#include <IconsMaterialDesign.h>

#include "actions/brushaction.h"
#include "actions/dropperaction.h"
#include "actions/eraseaction.h"
#include "actions/floodfillaction.h"
#include <iostream>

Tools::Tools()
    : _selectedTool(0)
{
    _tools.push_back(Tool({"Arrow", ICON_FA_MOUSE_POINTER, nullptr}));
    _tools.push_back(Tool({"Zoom", ICON_FA_SEARCH, nullptr}));
    _tools.push_back(Tool({"Deformation", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Crop", ICON_FA_CROP, nullptr}));
    _tools.push_back(Tool({"Mover", ICON_FA_ARROWS, nullptr}));
    _tools.push_back(Tool({"Selection", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Freehand", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Magic wand selection", ICON_FA_MAGIC, nullptr}));
    _tools.push_back(Tool({"Dropper", ICON_FA_EYEDROPPER, DropperActionFactory::Instance()}));
    _tools.push_back(Tool({"Paint brush", ICON_FA_PAINT_BRUSH, BrushActionFactory::Instance()}));
    _tools.push_back(Tool({"Clone brush", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Color replacer", ICON_FA_CLONE, nullptr}));
    _tools.push_back(Tool({"Retouch brush", ICON_FA_HAND_O_DOWN, nullptr}));
    //    _tools.push_back(Tool({"Scratch remover", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Erase", ICON_FA_ERASER, EraseActionFactory::Instance()}));
    //    _tools.push_back(Tool({"Picture tube", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Airbrush", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Flood fill", ICON_FA_TINT, FloodFillActionFactory::Instance()}));
    _tools.push_back(Tool({"Text", ICON_FA_FONT, nullptr}));
    _tools.push_back(Tool({"Draw", ICON_MD_GESTURE, nullptr}));
    _tools.push_back(Tool({"Preset shapes", ICON_FA_QUESTION, nullptr}));
    _tools.push_back(Tool({"Vector object selector", ICON_FA_QUESTION, nullptr}));
}

Tools::~Tools() {}

void Tools::selectTool(size_t index)
{
    this->_selectedTool = index;
    std::cout << "Selecting tool: " << index << std::endl;
}

size_t Tools::toolCount() const { return this->_tools.size(); }

const Tool &Tools::operator[](size_t index) const
{
    static Tool defaultTool;

    if (index < this->_tools.size())
    {
        return this->_tools[index];
    }

    return defaultTool;
}

const Tool &Tools::selectedTool() const
{
    static Tool defaultTool;

    if (this->_selectedTool < this->_tools.size())
    {
        return this->_tools[this->_selectedTool];
    }

    return defaultTool;
}

size_t Tools::selectedToolIndex() const
{
    return this->_selectedTool;
}

bool Tools::isSelected(size_t index) const
{
    return this->_selectedTool == index;
}
