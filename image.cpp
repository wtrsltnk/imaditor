#include "image.h"
#include "stb_image.h"

Image::Image() = default;

Image::~Image()
{
    if (_data != nullptr)
    {
        delete[] _data;
        _data = nullptr;
    }
}

Layer *Image::addLayer()
{
    byte pixel[4] = {255, 255, 255, 255};
    auto layer = Layer::defaultLayer(this->_size, pixel);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size() + 1);
    layer->setDirty();
    this->_layers.push_back(layer);

    return layer;
}

void Image::fromFile(const char *filename)
{
    auto layer = Layer::fromFile(filename);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size());
    layer->setDirty();
    this->_size[0] = layer->_size[0];
    this->_size[1] = layer->_size[1];
    this->_layers.push_back(layer);
}

void Image::selectLayer(size_t index)
{
    if (index >= 0 && index < this->_layers.size())
        _selectedLayer = index;
}

bool Image::isDirty() const
{
    if (_flags & Dirty) return true;

    for (Layer *layer : this->_layers)
        if (layer->_flags & Dirty) return true;

    return false;
}

void Image::removeCurrentLayer()
{
    if (this->_layers.size() > 1)
    {
        this->_layers.erase(this->_layers.begin() + this->_selectedLayer);
        if (this->_selectedLayer > 0) this->_selectedLayer--;
        this->setDirty();
    }
}

void Image::moveCurrentLayerUp()
{
    if (this->_selectedLayer >= 1)
    {
        auto tmp = this->_layers[this->_selectedLayer];
        this->_layers[this->_selectedLayer] = this->_layers[this->_selectedLayer - 1];
        this->_layers[this->_selectedLayer - 1] = tmp;
        this->_selectedLayer--;
        this->setDirty();
    }
}

void Image::moveCurrentLayerDown()
{
    if (this->_selectedLayer < this->_layers.size() - 1)
    {
        auto tmp = this->_layers[this->_selectedLayer];
        this->_layers[this->_selectedLayer] = this->_layers[this->_selectedLayer + 1];
        this->_layers[this->_selectedLayer + 1] = tmp;
        this->_selectedLayer++;
        this->setDirty();
    }
}
