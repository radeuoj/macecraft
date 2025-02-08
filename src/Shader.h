#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>

std::string get_file_contents(const char* filename);

class Shader
{
public:
    GLuint ID;
    Shader() = default;
    void init(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();

    void activate();

private:
    void compileErrors(unsigned int shader, const char* type);
};
