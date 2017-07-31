#include "brushes.h"

void Brushes::init()
{
    _count = 0;
    int size[2] = { 20, 20 };
    auto data = new unsigned char[size[0] * size[1] * 4];

    for (int y = 0; y < size[1]; ++y)
    {
        for (int x = 0; x < size[0]; ++x)
        {
            auto p = (x * size[1] + y);
            auto b = 0;
            data[p * 4 + b++] = x < 5 ? 0 : 255;
            data[p * 4 + b++] = 0;
            data[p * 4 + b++] = 255;
            data[p * 4 + b++] = 255;
        }
    }

    auto b = _brushes[_count++];
    glGenTextures(1, &(b._textureIndex));
    glBindTexture(GL_TEXTURE_2D, b._textureIndex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, size[0], size[1], 0, format, GL_UNSIGNED_BYTE, data);

}
