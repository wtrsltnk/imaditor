#include "brushaction.h"
#include "../image.h"

BrushAction::BrushAction() { }

BrushActionFactory::BrushActionFactory() { }

BrushActionFactory* BrushActionFactory::Instance()
{
    static BrushActionFactory instance;

    return &instance;
}

void BrushActionFactory::MouseMove(Image* image, int x, int y)
{
    _lastPosition[0] = x;
    _lastPosition[1] = y;
}

void BrushActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    // todo apply brush texture onto selected layer @ _lastPosition
}

void BrushActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void BrushActionFactory::SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void BrushActionFactory::SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

GLuint BrushActionFactory::ToolHelperImage()
{
    return 0;
}
