#pragma once

#include <glad\glad.h>
#include <string>
#include <fstream>

std::string get_file_contents(const char* filename);

class Shader
{
public:
    GLuint ID;
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);

    void Activate();
    void Delete();

private:
    void compileErrors(unsigned int shader, const char* type);
};
