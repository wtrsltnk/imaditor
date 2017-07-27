#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <GL/gl.h>

enum eFlags
{
    Dirty = 1 << 0,
};

class Layer
{
public:
    Layer();

    std::string _name;
    int _flags;
    bool _visible;
    bool isVisible() const { return _visible; }
    void toggleVisibility() { _visible = !_visible; setDirty(); }
    void setDirty() { _flags |= Dirty; }
    float _alpha;
    int _alphaMode;

    int _offset[2];
    int _size[2];
    int _bpp;
    unsigned char* _data;

    static Layer* defaultLayer(int size[2]);
    static Layer* fromFile(const char* filename);
};

class Image
{
public:
    Image();
    Image(const char* name);
    virtual ~Image();

    std::vector<Layer*> _layers;
    std::string _name;
    int _flags;
    std::string _fullPath;
    int _size[2];

    Layer* addLayer();
    void fromFile(const char* filename);
    bool isDirty() const;
    void setDirty() { _flags |= Dirty; }
    void selectLayer(int index);
    void removeCurrentLayer();
    void moveCurrentLayerUp();
    void moveCurrentLayerDown();

    unsigned char* _data;
    GLuint _glindex;

    int _selectedLayer;
};

class Images
{
    Image* _selectedImage;
public:
    Images();
    virtual ~Images();

    std::vector<Image*> _images;

    void select(int index);
    Image* selected();
    bool hasImages() const;
    void uploadSelectedImage();
};

#endif // IMAGE_H
