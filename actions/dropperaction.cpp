#include "dropperaction.h"
#include "../state.h"
#include "../image.h"

DropperAction::DropperAction() { }

DropperActionFactory::DropperActionFactory() : _isPainting(false) { }

DropperActionFactory* DropperActionFactory::Instance()
{
    static DropperActionFactory instance;

    return &instance;
}

void DropperActionFactory::getColor(Image* image, float color[], const glm::vec2& p)
{
    if (_isPainting)
    {
        auto layer = image->_layers[image->_selectedLayer];

        auto p = (_lastPosition[1] * layer->_size[1] + _lastPosition[0]);
        for (int b = 0; b < layer->_bpp; b++)
        {
            color[b] = layer->_data[p * layer->_bpp + b] / 255.0f;
        }

        layer->setDirty();
    }
}

void DropperActionFactory::MouseMove(Image* image, int x, int y)
{
    _lastPosition[0] = x;
    _lastPosition[1] = y;

    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    getColor(image, foreColor, glm::vec2(_lastPosition[0], _lastPosition[1]));
}

void DropperActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    _isPainting = true;
    getColor(image, foreColor, glm::vec2(_lastPosition[0], _lastPosition[1]));
}

void DropperActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    _isPainting = false;
}

void DropperActionFactory::SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void DropperActionFactory::SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

GLuint DropperActionFactory::ToolHelperImage()
{
    return 0;
}
