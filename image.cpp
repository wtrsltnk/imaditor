#include "image.h"
#include "stb_image.h"

Layer::Layer() : _flags(0), _visible(true), _alpha(1.0f), _alphaMode(0) { }

Layer* Layer::defaultLayer(int size[2])
{
    auto layer = new Layer();
    layer->_offset[0] = 0;
    layer->_offset[1] = 0;

    layer->_size[0] = size[0];
    layer->_size[1] = size[1];

    layer->_bpp = 4;

    auto dataSize = layer->_size[0] * layer->_size[1] * layer->_bpp;
    layer->_data = new unsigned char[dataSize];
    for (int i = 0; i < dataSize; ++i) layer->_data[i] = 255;

    return layer;
}

Layer* Layer::fromFile(const char* filename)
{
    auto layer = new Layer();

    int n = 4;
    layer->_data = stbi_load(filename, &(layer ->_size[0]), &(layer ->_size[1]), &(layer ->_bpp), 0);

    return layer;
}

Image::Image() : _flags(0), _data(nullptr), _glindex(0), _selectedLayer(0)
{
    _size[0] = _size[1] = 256.0f;
}

Image::~Image() { }

Layer* Image::addLayer()
{
    auto layer = Layer::defaultLayer(this->_size);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size() + 1);
    layer->setDirty();
    this->_layers.push_back(layer);

    return layer;
}

void Image::fromFile(const char* filename)
{
    auto layer = Layer::fromFile(filename);
    layer->_name = std::string("Layer ") + std::to_string(this->_layers.size());
    layer->setDirty();
    this->_size[0] = layer->_size[0];
    this->_size[1] = layer->_size[1];
    this->_layers.push_back(layer);
}

void Image::selectLayer(int index)
{
    if (index >= 0 && index < this->_layers.size())
        _selectedLayer = index;
}

bool Image::isDirty() const
{
    if (_flags & Dirty) return true;

    for (Layer* layer : this->_layers)
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
        this->_layers[this->_selectedLayer] = this->_layers[this->_selectedLayer-1];
        this->_layers[this->_selectedLayer-1] = tmp;
        this->_selectedLayer--;
        this->setDirty();
    }
}

void Image::moveCurrentLayerDown()
{
    if (this->_selectedLayer < this->_layers.size()-1)
    {
        auto tmp = this->_layers[this->_selectedLayer];
        this->_layers[this->_selectedLayer] = this->_layers[this->_selectedLayer+1];
        this->_layers[this->_selectedLayer+1] = tmp;
        this->_selectedLayer++;
        this->setDirty();
    }
}

Images::Images()
    : _selectedImage(nullptr)
{ }

Images::~Images() { }

void Images::select(int index)
{
    if (this->_selectedImage != nullptr)
    {
        glDeleteTextures(1, &(this->_selectedImage->_glindex));
        this->_selectedImage->_glindex = 0;
    }

    if (index >= 0 && index < this->_images.size())
    {
        this->_selectedImage = this->_images[index];
        uploadSelectedImage();
    }
}

Image* Images::selected()
{
    return this->_selectedImage;
}

bool Images::hasImages() const
{
    return this->_images.size() > 0;
}

void Images::uploadSelectedImage()
{
    auto img = this->_selectedImage;
    auto dataSize = img->_size[0] * img->_size[1] * 4;
    if (img->_data == nullptr) img->_data = new unsigned char[dataSize];

    memset(img->_data, 0, dataSize);

    for (Layer* layer : img->_layers)
    {
        if (!layer->_visible) continue;
        for (int y = 0; y < img->_size[1]; ++y)
        {
            for (int x = 0; x < img->_size[0]; ++x)
            {
                auto p = (x * img->_size[1] + y);
                for (int b = 0; b < layer->_bpp; b++)
                {
                    img->_data[p * 4 + b] += layer->_data[p * layer->_bpp + b];
                }
                if (layer->_bpp < 4) img->_data[p * 4 + 3] = 255;
            }
        }
    }

    if (img->_glindex == 0) glGenTextures(1, &(img->_glindex));
    glBindTexture(GL_TEXTURE_2D, img->_glindex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, img->_size[0], img->_size[1], 0, format, GL_UNSIGNED_BYTE, img->_data);
}
