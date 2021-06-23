#include "brushaction.h"
#include "../image.h"
#include "../state.h"

#include <iostream>

BrushAction::BrushAction() {}

BrushActionFactory::BrushActionFactory() = default;

BrushActionFactory *BrushActionFactory::Instance()
{
    static BrushActionFactory instance;

    return &instance;
}

void BrushActionFactory::paint(Layer *layer, float color[], const glm::vec2 &from, const glm::vec2 &to)
{
    if (!_isPainting)
    {
        return;
    }

    auto dir = to - from;
    auto ndir = glm::normalize(dir);

    glm::vec2 pixelPos = from;
    byte pixel[4];
    for (int b = 0; b < layer->_bpp; b++)
    {
        pixel[b] = static_cast<byte>(color[b] * 255.0f);
    }

    for (int i = 0; i < glm::length(dir) / glm::length(ndir); i++)
    {
        // todo : change this to writing the current brush at pixelPos
        layer->setPixel<4>(int(pixelPos.x), int(pixelPos.y), pixel);
        pixelPos += ndir;
    }

    layer->setDirty();
}

void BrushActionFactory::MouseMove(Image *image, int x, int y)
{
    auto from = glm::vec2(_lastPosition[0], _lastPosition[1]);
    auto to = glm::vec2(x, y);

    _lastPosition[0] = x;
    _lastPosition[1] = y;

    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[i] >= image->_size[i])
            return;

    paint(image->_layers[image->_selectedLayer], foreColor, from, to);
}

void BrushActionFactory::PrimaryMouseButtonDown(Image *image, bool shift, bool ctrl, bool alt, bool super)
{
    (void)shift;
    (void)ctrl;
    (void)alt;
    (void)super;

    for (int i = 0; i < 2; i++)
    {
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
        {
            return;
        }
    }

    _isPainting = true;

    auto to = glm::vec2(_lastPosition[0], _lastPosition[1]);

    paint(image->_layers[image->_selectedLayer], foreColor, to, to);
}

void BrushActionFactory::PrimaryMouseButtonUp(Image *image, bool shift, bool ctrl, bool alt, bool super)
{
    (void)image;
    (void)shift;
    (void)ctrl;
    (void)alt;
    (void)super;

    _isPainting = false;
}

void BrushActionFactory::SecondaryMouseButtonDown(Image *image, bool shift, bool ctrl, bool alt, bool super)
{
    (void)image;
    (void)shift;
    (void)ctrl;
    (void)alt;
    (void)super;
}

void BrushActionFactory::SecondaryMouseButtonUp(Image *image, bool shift, bool ctrl, bool alt, bool super)
{
    (void)image;
    (void)shift;
    (void)ctrl;
    (void)alt;
    (void)super;
}

GLuint BrushActionFactory::ToolHelperImage()
{
    return 0;
}
