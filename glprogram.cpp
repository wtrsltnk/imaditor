#include "glprogram.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

GlProgram::GlProgram() = default;

bool GlProgram::init(const std::string &vert, const std::string &frag)
{
    _program = LoadShaderProgram(vert.c_str(), frag.c_str());

    return _program > 0;
}

GlProgram &GlProgram::bind()
{
    glUseProgram(_program);

    return *this;
}

GlProgram &GlProgram::matrix(const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));

    return *this;
}
