#ifndef LAYER_H
#define LAYER_H

#include <string>

enum eFlags
{
    Dirty = 1 << 0,
};

typedef unsigned char byte;

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
    int dataSize() const;
    void setSize(int size[2]);
    void setSize(int w, int h);
    void setSize(int size[2], const byte pixel[]);
    void setSize(int w, int h, const byte pixel[]);
    template <int N> const byte* pixel(int x, int y) const
    {
        int position = y * _size[1] + x;
        return &this->_data[position * N];
    }
    template <int N> void setPixel(int x, int y, const byte pixel[])
    {
        int position = y * _size[1] + x;
        for (int b = 0; b < N; b++)
        {
            this->_data[position * N + b] = pixel[b];
        }
    }

    float _alpha;
    int _alphaMode;

    int _offset[2];
    int _size[2];
    int _bpp;
    unsigned char* _data;

    static Layer* defaultLayer(int size[2], const byte pixel[]);
    static Layer* fromFile(const char* filename);
    static void overwrite(Layer* a, Layer* b);
};

#endif // LAYER_H
