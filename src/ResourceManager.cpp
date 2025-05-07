#include "ResourceManager.h"
#include <iostream>

// Initialize the static instance
ResourceManager* ResourceManager::s_Instance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
    if (!s_Instance)
    {
        s_Instance = new ResourceManager();
    }
    return s_Instance;
}

ResourceManager::~ResourceManager()
{
    ReleaseAllShaders();
    ReleaseAllModels();
}

// Shader management functions
Shader* ResourceManager::GetShader(const std::string& name)
{
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end())
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
        m_Shaders[name] = std::move(shader);
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
        m_Shaders[name] = std::move(shader);
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
    m_Shaders.erase(name);
}

void ResourceManager::ReleaseAllShaders()
{
    m_Shaders.clear();
}

// Model management functions
Model* ResourceManager::GetModel(const std::string& name)
{
    auto it = m_Models.find(name);
    if (it != m_Models.end())
    {
        return it->second.get();
    }
    
    return nullptr;
}

Model* ResourceManager::LoadModel(const std::string& name, const std::string& filepath)
{
    // Check if the model already exists
    auto existingModel = GetModel(name);
    if (existingModel)
    {
        // Reload the model
        if (existingModel->LoadFromFile(filepath))
        {
            return existingModel;
        }
        else
        {
            std::cerr << "Failed to reload model '" << name << "' from file: " << filepath << std::endl;
            return nullptr;
        }
    }
    
    // Create a new model
    auto model = std::make_unique<Model>();
    if (model->LoadFromFile(filepath))
    {
        Model* rawPtr = model.get();
        m_Models[name] = std::move(model);
        return rawPtr;
    }
    else
    {
        std::cerr << "Failed to load model '" << name << "' from file: " << filepath << std::endl;
        return nullptr;
    }
}

void ResourceManager::ReleaseModel(const std::string& name)
{
    m_Models.erase(name);
}

void ResourceManager::ReleaseAllModels()
{
    m_Models.clear();
}