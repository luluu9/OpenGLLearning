#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Mesh.h"
#include "Shader.h"

// Forward declaration for Assimp
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

// Texture struct to be used with the Model class
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Model {
public:
    Model();
    ~Model();
    
    // Load the model from a file
    bool LoadFromFile(const std::string& path);
    
    // Draw the model
    void Draw(Shader* shader) const;
    
    // Check if the model is loaded
    bool IsLoaded() const { return m_IsLoaded; }
    
    // Get the filepath of the loaded model
    const std::string& GetFilePath() const { return m_FilePath; }
    
private:
    // Model data
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
    std::vector<Texture> m_LoadedTextures;
    std::string m_Directory;
    std::string m_FilePath;
    bool m_IsLoaded = false;
    
    // Functions to process Assimp model data
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, int type, const std::string& typeName);
    unsigned int TextureFromFile(const std::string& path);
};