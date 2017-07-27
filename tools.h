#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>

class Tool
{
public:
    std::string _name;
    const char* _icon;
    class BaseActionFactory* _actionFactory;
};

class Tools
{
    int _selectedTool;
    std::vector<Tool> _tools;

public:
    Tools();
    virtual ~Tools();

    void selectTool(int index);
    int toolCount() const;
    const Tool& selectedTool() const;
    int selectedToolIndex() const;
    bool isSelected(int index) const;
    const Tool& operator [] (int index) const;

};

#endif // TOOLS_H
