#include "brushes.h"
#include <glm/glm.hpp>

Brush::Brush() : _textureIndex(0) { }

void Brush::upload(int w, int h, int b, unsigned char *data)
{
    glGenTextures(1, &_textureIndex);
    glBindTexture(GL_TEXTURE_2D, _textureIndex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLenum format = GL_RGBA;
    if (b == 3) format = GL_RGB;
    if (b == 1) format = GL_ALPHA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Brushes::Brushes() : _count(0) { }

void Brushes::init()
{
    int size[2] = { 32, 32 };
    auto data = new unsigned char[size[0] * size[1]];

    for (int y = 0; y < size[1]; ++y)
    {
        for (int x = 0; x < size[0]; ++x)
        {
            auto p = (x * size[1] + y);
            data[p] = x > 5 && x < 27 && y > 5 && y < 27 ? 255 : 0;
        }
    }

    _brushes[_count].upload(size[0], size[1], 1, data);
    _count++;

    glm::vec2 c(size[0] / 2, size[1] / 2);
    for (int y = 0; y < size[1]; ++y)
    {
        for (int x = 0; x < size[0]; ++x)
        {
            glm::vec2 o(x, y);
            auto l = glm::length(c-o);
            auto p = (x * size[1] + y);
            data[p] = (unsigned char)(200.0f / l);
            if (o == c) data[p] = 255;
        }
    }

    _brushes[_count].upload(size[0], size[1], 1, data);
    _count++;

    delete [] data;
}
