#include "tools.h"
#include "font-icons.h"
#include "actions/floodfillaction.h"
#include "actions/brushaction.h"

#include <iostream>

Tools::Tools()
    : _selectedTool(0)
{
    _tools.push_back(Tool({ "Arrow", FontAwesomeIcons::FA_MOUSE_POINTER, nullptr }));
    _tools.push_back(Tool({ "Zoom", FontAwesomeIcons::FA_SEARCH, nullptr }));
    _tools.push_back(Tool({ "Deformation", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Crop", FontAwesomeIcons::FA_CROP, nullptr }));
    _tools.push_back(Tool({ "Mover", FontAwesomeIcons::FA_ARROWS, nullptr }));
    _tools.push_back(Tool({ "Selection", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Freehand", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Magic wand selection", FontAwesomeIcons::FA_MAGIC, nullptr }));
    _tools.push_back(Tool({ "Dropper", FontAwesomeIcons::FA_EYEDROPPER, nullptr }));
    _tools.push_back(Tool({ "Paint brush", FontAwesomeIcons::FA_PAINT_BRUSH, BrushActionFactory::Instance() }));
    _tools.push_back(Tool({ "Clone brush", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Color replacer", FontAwesomeIcons::FA_CLONE, nullptr }));
    _tools.push_back(Tool({ "Retouch brush", FontAwesomeIcons::FA_HAND_O_DOWN, nullptr }));
    _tools.push_back(Tool({ "Scratch remover", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Erase", FontAwesomeIcons::FA_ERASER, nullptr }));
    _tools.push_back(Tool({ "Picture tube", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Airbrush", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Flood fill", FontAwesomeIcons::FA_TINT, FloodFillActionFactory::Instance() }));
    _tools.push_back(Tool({ "Text", FontAwesomeIcons::FA_FONT, nullptr }));
    _tools.push_back(Tool({ "Draw", GoogleIcons::GI_GESTURE, nullptr }));
    _tools.push_back(Tool({ "Preset shapes", FontAwesomeIcons::FA_QUESTION, nullptr }));
    _tools.push_back(Tool({ "Vector object selector", FontAwesomeIcons::FA_QUESTION, nullptr }));
}

Tools::~Tools() { }

void Tools::selectTool(int index)
{
    this->_selectedTool = index;
    std::cout << "Selecting tool: " << index << std::endl;
}

int Tools::toolCount() const { return this->_tools.size(); }

const Tool& Tools::operator [] (int index) const
{
    static Tool defaultTool;

    if (index >= 0 && index < this->_tools.size())
    {
        return this->_tools[index];
    }

    return defaultTool;
}

const Tool& Tools::selectedTool() const
{
    static Tool defaultTool;

    if (this->_selectedTool >= 0 && this->_selectedTool < this->_tools.size())
    {
        return this->_tools[this->_selectedTool];
    }

    return defaultTool;
}

int Tools::selectedToolIndex() const
{
    return this->_selectedTool;
}

bool Tools::isSelected(int index) const
{
    return this->_selectedTool == index;
}
