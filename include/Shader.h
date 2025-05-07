#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader();
    ~Shader();

    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    void Use() const;
    void Delete();
    
    unsigned int GetID() const { return id; }
    
    // Uniform setters
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat2(const std::string& name, const glm::mat2& value) const;
    void SetMat3(const std::string& name, const glm::mat3& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;
    
    // Get uniform location with caching
    int GetUniformLocation(const std::string& name) const;
    
    // Get shader compilation log
    std::string GetCompilationLog() const { return compilationLog; }
    
private:
    unsigned int id = 0;
    mutable std::unordered_map<std::string, int> uniformLocationCache;
    std::string compilationLog;
    
    bool CompileShader(const std::string& vertexSource, const std::string& fragmentSource);
    unsigned int CompileShaderModule(unsigned int type, const std::string& source);
};