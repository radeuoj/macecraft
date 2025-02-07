#include "Shader.h"

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

Shader ShaderFromRes(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode = get_file_contents(vertexPath);
    std::string fragmentCode = get_file_contents(fragmentPath);

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    return Shader(vertexShaderSource, fragmentShaderSource);
}

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    compileErrors(vertexShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char *type)
{
    GLint success;

    char infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR\n" << infoLog << std::endl;
        }
    }
}




