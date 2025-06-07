#pragma once

#include "Shader.h"
#include "Model.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ResourceManager
{
public:
    // Shader metadata for categorization
    struct ShaderInfo {
        std::string name;
        std::string category;
        std::string vertexPath;
        std::string fragmentPath;
        std::string description;
    };

    static ResourceManager* GetInstance();
    
    // Shader management
    Shader* GetShader(const std::string& name);
    Shader* LoadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    Shader* LoadShaderWithTessellation(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath,
                                     const std::string& tessControlPath, const std::string& tessEvalPath);
    Shader* LoadShaderFromSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    void ReleaseShader(const std::string& name);
    void ReleaseAllShaders();
    
    // Shader library functions
    bool LoadAllShadersFromDirectory(const std::string& directory);
    std::vector<std::string> GetAllShaderNames() const;
    std::string GetShaderCategory(const std::string& name) const;
    const ShaderInfo& GetShaderInfo(const std::string& name) const;
    
    // Model management
    Model* GetModel(const std::string& name);
    Model* LoadModel(const std::string& name, const std::string& filepath);
    void ReleaseModel(const std::string& name);
    void ReleaseAllModels();
    
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
    std::unordered_map<std::string, ShaderInfo> shaderInfos;
    
    // Model cache
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    
    // Singleton instance
    static ResourceManager* instance;
};