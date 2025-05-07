#include "ResourceManager.h"
#include <iostream>

// Initialize the static instance
ResourceManager* ResourceManager::instance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
    if (!instance)
    {
        instance = new ResourceManager();
    }
    return instance;
}

ResourceManager::~ResourceManager()
{
    ReleaseAllShaders();
}

Shader* ResourceManager::GetShader(const std::string& name)
{
    auto it = shaders.find(name);
    if (it != shaders.end())
    {
        return it->second.get();
    }
    
    return nullptr;
}

Shader* ResourceManager::LoadShaderFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
    // Check if the shader already exists
    auto existingShader = GetShader(name);
    if (existingShader)
    {
        // Reload the shader
        if (existingShader->LoadFromFile(vertexPath, fragmentPath))
        {
            return existingShader;
        }
        else
        {
            std::cerr << "Failed to reload shader '" << name << "': " << existingShader->GetCompilationLog() << std::endl;
            return nullptr;
        }
    }
    
    // Create a new shader
    auto shader = std::make_unique<Shader>();
    if (shader->LoadFromFile(vertexPath, fragmentPath))
    {
        Shader* rawPtr = shader.get();
        shaders[name] = std::move(shader);
        return rawPtr;
    }
    else
    {
        std::cerr << "Failed to load shader '" << name << "': " << shader->GetCompilationLog() << std::endl;
        return nullptr;
    }
}

Shader* ResourceManager::LoadShaderFromSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
{
    // Check if the shader already exists
    auto existingShader = GetShader(name);
    if (existingShader)
    {
        // Reload the shader
        if (existingShader->LoadFromSource(vertexSource, fragmentSource))
        {
            return existingShader;
        }
        else
        {
            std::cerr << "Failed to reload shader '" << name << "': " << existingShader->GetCompilationLog() << std::endl;
            return nullptr;
        }
    }
    
    // Create a new shader
    auto shader = std::make_unique<Shader>();
    if (shader->LoadFromSource(vertexSource, fragmentSource))
    {
        Shader* rawPtr = shader.get();
        shaders[name] = std::move(shader);
        return rawPtr;
    }
    else
    {
        std::cerr << "Failed to load shader '" << name << "': " << shader->GetCompilationLog() << std::endl;
        return nullptr;
    }
}

void ResourceManager::ReleaseShader(const std::string& name)
{
    shaders.erase(name);
}

void ResourceManager::ReleaseAllShaders()
{
    shaders.clear();
}