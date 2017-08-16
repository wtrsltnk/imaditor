#include "eraseaction.h"
#include "../state.h"
#include "../image.h"

EraseAction::EraseAction() { }

EraseActionFactory::EraseActionFactory() : _isErasing(false) { }

EraseActionFactory* EraseActionFactory::Instance()
{
    static EraseActionFactory instance;

    return &instance;
}

void EraseActionFactory::erase(Image* image, float color[], const glm::vec2& p)
{
    if (_isErasing)
    {
        auto layer = image->_layers[image->_selectedLayer];

        auto p = (_lastPosition[1] * layer->_size[1] + _lastPosition[0]);
        for (int b = 0; b < layer->_bpp; b++)
        {
            layer->_data[p * layer->_bpp + b] = 0.0f;
        }

        layer->setDirty();
    }
}

void EraseActionFactory::MouseMove(Image* image, int x, int y)
{
    _lastPosition[0] = x;
    _lastPosition[1] = y;

    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    erase(image, foreColor, glm::vec2(_lastPosition[0], _lastPosition[1]));
}

void EraseActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    _isErasing = true;
    erase(image, foreColor, glm::vec2(_lastPosition[0], _lastPosition[1]));
}

void EraseActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    _isErasing = false;
}

void EraseActionFactory::SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void EraseActionFactory::SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

GLuint EraseActionFactory::ToolHelperImage()
{
    return 0;
}
