#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include "layer.h"

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

    int _selectedLayer;
};

#endif // IMAGE_H
