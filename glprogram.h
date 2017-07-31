#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <initializer_list>
#include <string>
#include <GL/gl.h>
#include <glm/glm.hpp>

class GlProgram
{
    GLuint _program;
public:
    GlProgram();

    bool init(const std::string& vert, const std::string& frag);

    GlProgram& bind();
    GlProgram& matrix(const std::string& name, const glm::mat4& mat);
};

#endif // GLPROGRAM_H
