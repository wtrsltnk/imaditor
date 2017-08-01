#ifndef BRUSHES_H
#define BRUSHES_H

#include <GL/glextl.h>

class Brush
{
public:
    Brush();

    void upload(int w, int h, int b, unsigned char* data);

    GLuint _textureIndex;
};

class Brushes
{
public:
    Brushes();

    void init();

    int _count;
    Brush _brushes[10];
};

#endif // BRUSHES_H
