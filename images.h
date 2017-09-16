#ifndef IMAGES_H
#define IMAGES_H

#include <GL/gl.h>
#include <map>
#include "image.h"

class UploadableImage
{
public:
    UploadableImage(Image* img);
    GLuint _glindex;
    Image* image;
};

class Images
{
    UploadableImage* _selectedImage;
public:
    Images();
    virtual ~Images();

    std::vector<UploadableImage> _images;

    void addImage(Image* img);
    void select(int index);
    Image* selected();
    bool hasImages() const;
    void uploadSelectedImage();
};

#endif // IMAGES_H
