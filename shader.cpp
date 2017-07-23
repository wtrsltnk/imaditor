// Make sure we include stdio first to make fopen to work on android
#include <stdio.h>
#include <memory.h>

#include "shader.h"
#include "log.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

char* readWholeFile(const std::string& filename)
{
    auto file = fopen(filename.c_str(), "r");
    if (file != nullptr)
    {
        fseek(file, 0, SEEK_END);
        auto size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* buffer = (char*)malloc(size + 1);
        memset(buffer, 0, size + 1);
        fread(buffer, sizeof(char), size + 1, file);
        fclose(file);

        return buffer;
    }

    return nullptr;
}

GLuint LoadShaderProgram(const char* vertShaderSrc, const char* fragShaderSrc)
{
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    GLint logLength;

    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        Log::Current().Error("compiling vertex shader");
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> vertShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));
        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
        Log::Current().Error(&vertShaderError[0]);
    }

    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        Log::Current().Error("compiling fragment shader");
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));
        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
        Log::Current().Error(&fragShaderError[0]);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        Log::Current().Error("linking program");
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> programError(static_cast<size_t>((logLength > 1) ? logLength : 1));
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        Log::Current().Error(&programError[0]);
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}
