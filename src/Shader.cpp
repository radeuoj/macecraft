#include "Shader.h"

#include <iostream>

namespace Macecraft
{
    
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

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    CompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    CompileErrors(vertexShader, "FRAGMENT");

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);
    CompileErrors(m_ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
    glUseProgram(m_ID);
}

Shader::~Shader()
{
    glDeleteProgram(m_ID);
}

void Shader::CompileErrors(unsigned int shader, const char *type)
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

GLuint Shader::GetID() const
{
    return m_ID;
}
;
    
}





