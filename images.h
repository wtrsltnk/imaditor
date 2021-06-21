#ifndef IMAGES_H
#define IMAGES_H

#include <glad/glad.h>

#include "image.h"
#include <map>

class UploadableImage
{
public:
    UploadableImage(Image *img);
    GLuint _glindex = 0;
    Image *image = nullptr;
};

class Images
{
    UploadableImage *_selectedImage = nullptr;

public:
    Images();
    virtual ~Images();

    std::vector<UploadableImage> _images;

    void addImage(Image *img);
    void select(size_t index);
    Image *selected();
    bool hasImages() const;
    void uploadSelectedImage();
};

#endif // IMAGES_H
