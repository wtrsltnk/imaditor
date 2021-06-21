#ifndef IMAGE_H
#define IMAGE_H

#include "layer.h"
#include <string>
#include <vector>

class Image
{
public:
    Image();
    Image(const char *name);
    virtual ~Image();

    std::vector<Layer *> _layers;
    std::string _name;
    int _flags = 0;
    std::string _fullPath;
    int _size[2] = {256, 256};

    Layer *addLayer();
    void fromFile(const char *filename);
    bool isDirty() const;
    void setDirty() { _flags |= Dirty; }
    void selectLayer(size_t index);
    void removeCurrentLayer();
    void moveCurrentLayerUp();
    void moveCurrentLayerDown();

    unsigned char *_data = nullptr;

    size_t _selectedLayer = 0;
};

#endif // IMAGE_H
