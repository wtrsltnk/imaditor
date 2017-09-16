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

void BrushActionFactory::paint(float color[], const glm::vec2& from, const glm::vec2& to)
{
    if (_isPainting)
    {
        auto dir = to - from;
        auto ndir = glm::normalize(dir);

        glm::vec2 pixelPos = from;
        byte pixel[4];
        for (int b = 0; b < this->_tmpLayer._bpp; b++) pixel[b] = color[b] * 255;
        for (int i = 0; i < glm::length(dir) / glm::length(ndir); i++)
        {
            // todo : change this to writing the current brush at pixelPos
            this->_tmpLayer.setPixel<4>(int(pixelPos.x), int(pixelPos.y), pixel);
            pixelPos += ndir;
        }

        this->_tmpLayer.setDirty();
    }
}

void BrushActionFactory::MouseMove(Image* image, int x, int y)
{
    auto from = glm::vec2(_lastPosition[0], _lastPosition[1]);
    _lastPosition[0] = x;
    _lastPosition[1] = y;
    auto to = glm::vec2(_lastPosition[0], _lastPosition[1]);

    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    if (_isPainting)
    {
        paint(foreColor, from, to);
    }
}

void BrushActionFactory::PrimaryMouseButtonDown(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    for (int i = 0; i < 2; i++)
        if (_lastPosition[i] < 0 || _lastPosition[1] >= image->_size[1])
            return;

    _isPainting = true;
    auto to = glm::vec2(_lastPosition[0], _lastPosition[1]);
    byte pixel[4] = { 0, 0, 0, 0 };
    _tmpLayer.setSize(image->_size, pixel);
    paint(foreColor, to, to);
}

void BrushActionFactory::PrimaryMouseButtonUp(Image* image, bool shift, bool ctrl, bool alt, bool super)
{
    Layer::overwrite(image->_layers[image->_selectedLayer], &_tmpLayer);

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
