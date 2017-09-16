#include "layer.h"
#include "stb_image.h"

Layer::Layer() : _data(nullptr), _flags(0), _visible(true), _alpha(1.0f), _alphaMode(0)
{
    _size[0] = _size[1] = 0;
}

void Layer::setSize(int size[2])
{
    byte pixel[4] = { 255, 255, 255, 255 };
    setSize(size[0], size[1], pixel);
}

void Layer::setSize(int w, int h)
{
    byte pixel[4] = { 255, 255, 255, 255 };
    setSize(w, h, pixel);
}

void Layer::setSize(int size[2], const byte pixel[])
{
    setSize(size[0], size[1], pixel);
}

void Layer::setSize(int w, int h, const byte pixel[])
{
    this->_offset[0] = 0;
    this->_offset[1] = 0;

    this->_bpp = 4;

    if (w != _size[0] || h != _size[1])
    {
        this->_size[0] = w;
        this->_size[1] = h;

        if (this->_data != nullptr)
        {
            delete []this->_data;
        }

        this->_data = new unsigned char[dataSize()];
    }

    for (int y = 0; y < this->_size[1]; y++)
    {
        for (int x = 0; x < this->_size[0]; x++)
        {
            setPixel<4>(x, y, pixel);
        }
    }
}

int Layer::dataSize() const
{
    return this->_size[0] * this->_size[1] * this->_bpp;
}

Layer* Layer::defaultLayer(int size[2], const byte pixel[])
{
    auto layer = new Layer();

    layer->setSize(size, pixel);

    return layer;
}

Layer* Layer::fromFile(const char* filename)
{
    auto layer = new Layer();

    int n = 4;
    layer->_data = stbi_load(filename, &(layer ->_size[0]), &(layer ->_size[1]), &(layer ->_bpp), 0);

    return layer;
}

void Layer::overwrite(Layer* a, Layer* b)
{
    for (int y = 0; y < b->_size[1]; y++)
    {
        for (int x = 0; x < b->_size[0]; x++)
        {
            if (x + a->_offset[0] < 0) continue;
            if (y + a->_offset[1] < 0) continue;
            if (x + b->_offset[0] < 0) continue;
            if (y + b->_offset[1] < 0) continue;
            if (x + a->_offset[0] >= a->_size[0]) continue;
            if (y + a->_offset[1] >= a->_size[1]) continue;
            if (x + b->_offset[0] >= b->_size[0]) continue;
            if (y + b->_offset[1] >= b->_size[1]) continue;

            auto bpixel = b->pixel<4>(x + b->_offset[0], y + b->_offset[1]);
            if (bpixel[3] == 0) continue;
            a->setPixel<4>(x + a->_offset[0], y + a->_offset[1], bpixel);
        }
    }
}
