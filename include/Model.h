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
    
    bool LoadFromFile(const std::string& path);
    void Draw(Mesh::RenderMode mode = Mesh::RenderMode::TRIANGLES) const;
    
    bool IsLoaded() const { return isLoaded; }
    const std::string& GetFilePath() const { return filepath; }
    
private:
    // Model data
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Texture> loadedTextures;
    std::string directory;
    std::string filepath;
    bool isLoaded = false;
    
    // Assimp model data processing
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, int type, const std::string& typeName);
    unsigned int TextureFromFile(const std::string& path);
};