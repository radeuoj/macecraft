#include "shader.h"

#include <iostream>

std::string get_file_contents(const char *filename)
{
    std::ifstream fin(filename, std::ios::binary);

    if (fin)
    {
        std::string contents;
        fin.seekg(0, std::ios::end);
        contents.resize(fin.tellg());
        fin.seekg(0, std::ios::beg);
        fin.read(&contents[0], contents.size());
        fin.close();

        // std::cout << contents << std::endl;

        return contents;
    }

    std::cout << "COULDN'T OPEN FILE" << ' ' << filename << '\n';

    throw errno;
}

Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode = get_file_contents(vertexPath);
    std::string fragmentCode = get_file_contents(fragmentPath);

    const char *vertexShaderSource = vertexCode.c_str();
    const char *fragmentShaderSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}



