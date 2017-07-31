#ifndef BRUSHES_H
#define BRUSHES_H

#include <GL/glextl.h>

class Brush
{
public:
    GLuint _textureIndex;
};

class Brushes
{
public:
    void init();

    int _count;
    Brush _brushes[10];
};

#endif // BRUSHES_H
