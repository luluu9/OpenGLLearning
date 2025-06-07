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
        compilationLog = "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " + std::string(e.what());
        std::cerr << compilationLog << std::endl;
        return false;
    }
    
    return LoadFromSource(vertexCode, fragmentCode);
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource)
{
    // Delete the previous shader if one exists
    if (id != 0)
        Delete();
        
    return CompileShader(vertexSource, fragmentSource);
}

void Shader::Use() const
{
    glUseProgram(id);
}

void Shader::Delete()
{
    if (id != 0)
    {
        glDeleteProgram(id);
        id = 0;
    }
    uniformLocationCache.clear();
}

bool Shader::CompileShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    compilationLog.clear();
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
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    
    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        compilationLog += "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog);
        std::cerr << compilationLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(id);
        id = 0;
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
        compilationLog += "ERROR::SHADER::" + 
            std::string(type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") + 
            "::COMPILATION_FAILED\n" + std::string(infoLog);
        std::cerr << compilationLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool Shader::LoadWithTessellationFromFile(const std::string& vertexPath, const std::string& fragmentPath,
                                         const std::string& tessControlPath, const std::string& tessEvalPath)
{
    std::string vertexCode, fragmentCode, tessControlCode, tessEvalCode;
    std::ifstream vShaderFile, fShaderFile, tcShaderFile, teShaderFile;
    
    // Ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        tcShaderFile.open(tessControlPath);
        teShaderFile.open(tessEvalPath);
        
        // Read file contents into streams
        std::stringstream vShaderStream, fShaderStream, tcShaderStream, teShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        tcShaderStream << tcShaderFile.rdbuf();
        teShaderStream << teShaderFile.rdbuf();
        
        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();
        tcShaderFile.close();
        teShaderFile.close();
        
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        tessControlCode = tcShaderStream.str();
        tessEvalCode = teShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        compilationLog = "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " + std::string(e.what());
        std::cerr << compilationLog << std::endl;
        return false;
    }
    
    return LoadWithTessellationFromSource(vertexCode, fragmentCode, tessControlCode, tessEvalCode);
}

bool Shader::LoadWithTessellationFromSource(const std::string& vertexSource, const std::string& fragmentSource,
                                           const std::string& tessControlSource, const std::string& tessEvalSource)
{
    if (id != 0)
        Delete();
        
    return CompileShaderWithTessellation(vertexSource, fragmentSource, tessControlSource, tessEvalSource);
}

bool Shader::CompileShaderWithTessellation(const std::string& vertexSource, const std::string& fragmentSource,
                                          const std::string& tessControlSource, const std::string& tessEvalSource)
{
    compilationLog.clear();
    
    // Compile all shader stages
    unsigned int vertexShader = CompileShaderModule(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0)
        return false;
        
    unsigned int tessControlShader = CompileShaderModule(GL_TESS_CONTROL_SHADER, tessControlSource);
    if (tessControlShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    unsigned int tessEvalShader = CompileShaderModule(GL_TESS_EVALUATION_SHADER, tessEvalSource);
    if (tessEvalShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(tessControlShader);
        return false;
    }
    
    unsigned int fragmentShader = CompileShaderModule(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(tessControlShader);
        glDeleteShader(tessEvalShader);
        return false;
    }
    
    // Create shader program
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, tessControlShader);
    glAttachShader(id, tessEvalShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    
    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        compilationLog += "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog);
        std::cerr << compilationLog << std::endl;
        
        // Delete all shader modules
        glDeleteShader(vertexShader);
        glDeleteShader(tessControlShader);
        glDeleteShader(tessEvalShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(id);
        id = 0;
        return false;
    }
    
    // Delete shaders as they're linked into the program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(tessControlShader);
    glDeleteShader(tessEvalShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

int Shader::GetUniformLocation(const std::string& name) const
{
    // Check if the uniform location is already in the cache
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
        return uniformLocationCache[name];
        
    // Get the location from OpenGL
    int location = glGetUniformLocation(id, name.c_str());
    
    // Warn if uniform not found (but don't error, as it might be unused by the shader)
    if (location == -1) {
        // Only log if we're not checking for optional uniforms
        if (name != "lightPos" && name != "lightColor" && name != "lightIntensity") {
            std::cerr << "Warning: Uniform '" << name << "' not found in shader " << this->name << std::endl;
        }
    }
    
    uniformLocationCache[name] = location;
    return location;
}

void Shader::SetBool(const std::string& name, bool value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniform1i(location, static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniform1i(location, value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniform1f(location, value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::SetMat2(const std::string& name, const glm::mat2& value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    int location = GetUniformLocation(name);
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}