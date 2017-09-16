#include "images.h"
#include "stb_image.h"

UploadableImage::UploadableImage(Image* img) : image(img) { }

Images::Images()
    : _selectedImage(nullptr)
{ }

Images::~Images() { }

void Images::addImage(Image* img)
{
    _images.push_back(UploadableImage(img));
}

void Images::select(int index)
{
    if (this->_selectedImage != nullptr)
    {
        glDeleteTextures(1, &(this->_selectedImage->_glindex));
        this->_selectedImage->_glindex = 0;
    }

    if (index >= 0 && index < this->_images.size())
    {
        this->_selectedImage = &this->_images[index];
        uploadSelectedImage();
    }
}

Image* Images::selected()
{
    if (this->_selectedImage != nullptr)
    {
        return this->_selectedImage->image;
    }

    return nullptr;
}

bool Images::hasImages() const
{
    return this->_images.size() > 0;
}

void Images::uploadSelectedImage()
{
    auto img = this->_selectedImage;
    auto dataSize = img->image->_size[0] * img->image->_size[1] * 4;
    if (img->image->_data == nullptr) img->image->_data = new unsigned char[dataSize];

    memset(img->image->_data, 0, dataSize);

    for (Layer* layer : img->image->_layers)
    {
        if (!layer->_visible) continue;
        for (int y = 0; y < img->image->_size[1]; ++y)
        {
            for (int x = 0; x < img->image->_size[0]; ++x)
            {
                auto p = (x * img->image->_size[1] + y);
                for (int b = 0; b < layer->_bpp; b++)
                {
                    img->image->_data[p * 4 + b] += layer->_data[p * layer->_bpp + b];
                }
                if (layer->_bpp < 4) img->image->_data[p * 4 + 3] = 255;
            }
        }
    }

    if (img->_glindex == 0) glGenTextures(1, &(img->_glindex));
    glBindTexture(GL_TEXTURE_2D, img->_glindex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, img->image->_size[0], img->image->_size[1], 0, format, GL_UNSIGNED_BYTE, img->image->_data);
}
