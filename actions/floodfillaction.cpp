#include "floodfillaction.h"
#include "../state.h"
#include "../image.h"

FloodFillAction::FloodFillAction()
{ }

FloodFillActionFactory::FloodFillActionFactory()
{ }

FloodFillActionFactory* FloodFillActionFactory::Instance()
{
    static FloodFillActionFactory instance;

    return &instance;
}

void FloodFillActionFactory::MouseMove(Image* image, int x, int y)
{ }

void FloodFillActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void FloodFillActionFactory::floodFill(class Layer* layer, float color[])
{
    for (int y = 0; y < layer->_size[1]; ++y)
    {
        for (int x = 0; x < layer->_size[0]; ++x)
        {
            auto p = (x * layer->_size[1] + y);
            for (int b = 0; b < layer->_bpp; b++)
            {
                layer->_data[p * layer->_bpp + b] = color[b] * 255;
            }
        }
    }
}

void FloodFillActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    auto layer = image->_layers[image->_selectedLayer];
    floodFill(layer, foreColor);
    layer->setDirty();
}

void FloodFillActionFactory::SecondaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{ }

void FloodFillActionFactory::SecondaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    auto layer = image->_layers[image->_selectedLayer];
    floodFill(layer, backColor);
    layer->setDirty();
}

GLuint FloodFillActionFactory::ToolHelperImage()
{
    return 0;
}
