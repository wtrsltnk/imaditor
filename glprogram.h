#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <initializer_list>
#include <string>

class GlProgram
{
    GLuint _program = 0;

public:
    GlProgram();

    bool init(const std::string &vert, const std::string &frag);

    GlProgram &bind();
    GlProgram &matrix(const std::string &name, const glm::mat4 &mat);
};

#endif // GLPROGRAM_H
