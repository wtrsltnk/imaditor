#include "brushaction.h"
#include "../state.h"
#include "../image.h"

BrushAction::BrushAction() { }

BrushActionFactory::BrushActionFactory() : _isPainting(false) { }

BrushActionFactory* BrushActionFactory::Instance()
{
    static BrushActionFactory instance;

    return &instance;
}

void BrushActionFactory::paint(Image* image, float color[], const glm::vec2& p)
{
    if (_isPainting)
    {
        auto layer = image->_layers[image->_selectedLayer];

        auto p = (_lastPosition[1] * layer->_size[1] + _lastPosition[0]);
        for (int b = 0; b < layer->_bpp; b++)
        {
            layer->_data[p * layer->_bpp + b] = color[b] * 255;
        }

        layer->setDirty();
    }
}

void BrushActionFactory::MouseMove(Image* image, int x, int y)
{
    _lastPosition[0] = x;
    _lastPosition[1] = y;

    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    if (_isPainting)
    {
        paint(image, foreColor, glm::vec2(_lastPosition[0], _lastPosition[1]));
    }
}

void BrushActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    _isPainting = true;
    paint(image, foreColor, glm::vec2(_lastPosition[0], _lastPosition[1]));
}

void BrushActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    _isPainting = false;
}

void BrushActionFactory::SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void BrushActionFactory::SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

GLuint BrushActionFactory::ToolHelperImage()
{
    return 0;
}
