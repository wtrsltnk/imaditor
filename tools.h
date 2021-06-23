#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>

class Tool
{
public:
    std::string _name;
    const char *_icon = nullptr;
    class BaseActionFactory *_actionFactory = nullptr;
};

class Tools
{
    size_t _selectedTool = 0;
    std::vector<Tool> _tools;

public:
    Tools();
    virtual ~Tools();

    void selectTool(size_t index);
    size_t toolCount() const;
    const Tool &selectedTool() const;
    size_t selectedToolIndex() const;
    bool isSelected(size_t index) const;
    const Tool &operator[](size_t index) const;
};

#endif // TOOLS_H
