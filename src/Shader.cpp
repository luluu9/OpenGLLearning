#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() = default;

Shader::~Shader()
{
    Delete();
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    // Ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        
        // Read file contents into streams
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        m_CompilationLog = "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " + std::string(e.what());
        std::cerr << m_CompilationLog << std::endl;
        return false;
    }
    
    return LoadFromSource(vertexCode, fragmentCode);
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource)
{
    // Delete the previous shader if one exists
    if (m_ID != 0)
        Delete();
        
    return CompileShader(vertexSource, fragmentSource);
}

void Shader::Use() const
{
    glUseProgram(m_ID);
}

void Shader::Delete()
{
    if (m_ID != 0)
    {
        glDeleteProgram(m_ID);
        m_ID = 0;
    }
    m_UniformLocationCache.clear();
}

bool Shader::CompileShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    m_CompilationLog.clear();
    unsigned int vertexShader = CompileShaderModule(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0)
        return false;
        
    unsigned int fragmentShader = CompileShaderModule(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // Create shader program
    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);
    
    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        m_CompilationLog += "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog);
        std::cerr << m_CompilationLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_ID);
        m_ID = 0;
        return false;
    }
    
    // Delete shaders as they're linked into the program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

unsigned int Shader::CompileShaderModule(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    
    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        m_CompilationLog += "ERROR::SHADER::" + 
            std::string(type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") + 
            "::COMPILATION_FAILED\n" + std::string(infoLog);
        std::cerr << m_CompilationLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

int Shader::GetUniformLocation(const std::string& name) const
{
    // Check if the uniform location is already in the cache
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
        
    // Get the location from OpenGL
    int location = glGetUniformLocation(m_ID, name.c_str());
    m_UniformLocationCache[name] = location;
    
    return location;
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat2(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}