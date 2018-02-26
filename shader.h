#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>

GLuint LoadShaderProgram(const std::string& shaderName);
GLuint LoadShaderProgram(const char* vertShaderSrc, const char* fragShaderSrc);

#endif // SHADER_H
