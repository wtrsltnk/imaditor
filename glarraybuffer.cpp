#include "glarraybuffer.h"

static float g_vertex_buffer_data[] = {
    0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  0.0f,
    0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  0.0f,
    -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f,
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  0.0f,
};

GlArrayBuffer::GlArrayBuffer()
{ }

void GlArrayBuffer::init()
{
    glGenBuffers(1, &_index);
    glBindBuffer(GL_ARRAY_BUFFER, _index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GlArrayBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, _index);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void GlArrayBuffer::render()
{
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GlArrayBuffer::unbind()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
