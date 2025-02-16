#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>

namespace Macecraft
{
    
std::string get_file_contents(const char* filename);

class Shader
{
public:
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    Shader(const Shader& other) = delete;
    Shader(Shader&& other) = delete;
    ~Shader();

    void Activate();
    GLuint GetID() const;

private:
    GLuint m_ID;
    
    void CompileErrors(unsigned int shader, const char* type);
};
    
}

