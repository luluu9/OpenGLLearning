#include "ResourceManager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <functional>

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
    ReleaseAllModels();
}

// Shader management functions
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
    shader->SetName(name);
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
    shader->SetName(name);
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

Model* ResourceManager::GetModel(const std::string& name)
{
    auto it = models.find(name);
    if (it != models.end())
    {
        return it->second.get();
    }
    
    return nullptr;
}

Model* ResourceManager::LoadModel(const std::string& name, const std::string& filepath)
{
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
        models[name] = std::move(model);
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
    models.erase(name);
}

void ResourceManager::ReleaseAllModels()
{
    models.clear();
}

// Shader Library Functions
bool ResourceManager::LoadAllShadersFromDirectory(const std::string& directory)
{
    namespace fs = std::filesystem;
    
    if (!fs::exists(directory) || !fs::is_directory(directory))
    {
        std::cerr << "Error: Shader directory does not exist: " << directory << std::endl;
        return false;
    }
    
    // Map to store pairs of vertex and fragment shaders
    std::unordered_map<std::string, std::pair<std::string, std::string>> shaderPairs;
    
    // Process all files in the directory and its subdirectories
    std::function<void(const fs::path&)> processDirectory = [&](const fs::path& dirPath) {
        try {
            // Step 1: Find all shader files and organize them by base name
            for (const auto& entry : fs::directory_iterator(dirPath))
            {
                if (entry.is_directory())
                {
                    // Process subdirectory recursively
                    processDirectory(entry.path());
                    continue;
                }
                
                if (!entry.is_regular_file())
                    continue;
                    
                const auto& path = entry.path();
                std::string filePath = path.string();
                std::string extension = path.extension().string();
                std::string baseName = path.stem().string();
                  // Determine shader category based on parent directory relative to base shader directory
                std::string category = "Default";
                fs::path parentDir = path.parent_path();
                fs::path relativePath = fs::relative(parentDir, fs::path(directory));
                
                if (!relativePath.empty() && relativePath.string() != ".")
                {
                    // Use the first subdirectory as the category
                    category = relativePath.begin()->string();
                }
                  // Check if this is a vertex or fragment shader
                if (extension == ".vert")
                {
                    // Use full path for uniqueness but category+basename for display
                    std::string fullName = baseName;
                    if (category != "Default") {
                        fullName = category + "/" + baseName;
                    }
                    
                    // If we already have a fragment shader for this base name, create a pair
                    if (shaderPairs.find(fullName) != shaderPairs.end())
                    {
                        shaderPairs[fullName].first = filePath;
                    }
                    else
                    {
                        shaderPairs[fullName] = std::make_pair(filePath, "");
                    }
                    
                    // Store shader metadata
                    
                    shaderInfos[fullName].name = baseName;
                    shaderInfos[fullName].vertexPath = filePath;
                    shaderInfos[fullName].category = category;
                    
                    // Try to extract description from the shader file
                    std::ifstream file(filePath);
                    if (file)
                    {
                        std::string line;
                        while (std::getline(file, line))
                        {
                            std::regex descRegex("//\\s*Description:\\s*(.+)");
                            std::smatch match;
                            if (std::regex_search(line, match, descRegex) && match.size() > 1)
                            {
                                shaderInfos[fullName].description = match[1].str();
                                break;
                            }
                        }
                    }
                }
                else if (extension == ".frag")
                {
                    // Use full path for uniqueness but category+basename for display
                    std::string fullName = baseName;
                    if (category != "Default") {
                        fullName = category + "/" + baseName;
                    }
                    
                    // If we already have a vertex shader for this base name, create a pair
                    if (shaderPairs.find(fullName) != shaderPairs.end())
                    {
                        shaderPairs[fullName].second = filePath;
                    }
                    else
                    {
                        shaderPairs[fullName] = std::make_pair("", filePath);
                    }
                    
                    // Store shader metadata
                    shaderInfos[fullName].name = baseName;
                    shaderInfos[fullName].fragmentPath = filePath;
                    shaderInfos[fullName].category = category;
                      // Try to extract description if we haven't found one yet
                    if (shaderInfos[fullName].description.empty())
                    {
                        std::ifstream file(filePath);
                        if (file)
                        {
                            std::string line;
                            while (std::getline(file, line))
                            {
                                std::regex descRegex("//\\s*Description:\\s*(.+)");
                                std::smatch match;
                                if (std::regex_search(line, match, descRegex) && match.size() > 1)
                                {
                                    shaderInfos[fullName].description = match[1].str();
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing directory " << dirPath << ": " << e.what() << std::endl;
        }
    };
    
    // Start the recursive processing from the base directory
    processDirectory(directory);
    
    // Step 2: Load all shader pairs that have both vertex and fragment shaders
    bool allLoaded = true;
    for (const auto& [name, paths] : shaderPairs)
    {
        if (!paths.first.empty() && !paths.second.empty())
        {
            Shader* shader = LoadShaderFromFile(name, paths.first, paths.second);
            if (!shader)
            {
                std::cerr << "Failed to load shader pair: " << name << std::endl;
                allLoaded = false;
            }
        }
        else
        {
            std::cerr << "Incomplete shader pair for: " << name << std::endl;
            if (paths.first.empty())
                std::cerr << "  Missing vertex shader (.vert)" << std::endl;
            if (paths.second.empty())
                std::cerr << "  Missing fragment shader (.frag)" << std::endl;
            allLoaded = false;
        }
    }
    
    return allLoaded;
}

std::vector<std::string> ResourceManager::GetAllShaderNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, _] : shaders)
    {
        names.push_back(name);
    }
    return names;
}

std::string ResourceManager::GetShaderCategory(const std::string& name) const
{
    auto it = shaderInfos.find(name);
    if (it != shaderInfos.end())
    {
        return it->second.category;
    }
    return "Uncategorized";
}

const ResourceManager::ShaderInfo& ResourceManager::GetShaderInfo(const std::string& name) const
{
    static const ShaderInfo emptyInfo;
    
    auto it = shaderInfos.find(name);
    if (it != shaderInfos.end())
    {
        return it->second;
    }
    return emptyInfo;
}