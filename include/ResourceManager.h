#pragma once

#include "Shader.h"
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    static ResourceManager* GetInstance();
    
    // Shader management
    Shader* GetShader(const std::string& name);
    Shader* LoadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    Shader* LoadShaderFromSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    void ReleaseShader(const std::string& name);
    void ReleaseAllShaders();
    
private:
    ResourceManager() {}
    ~ResourceManager();
    
    // Delete copy and move constructors and assignment operators
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    
    // Shader cache
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    
    // Singleton instance
    static ResourceManager* instance;
};