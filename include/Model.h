#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Mesh.h"
#include "Shader.h"

struct aiScene;
struct aiNode;
struct aiMesh;

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
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Texture> loadedTextures;
    std::string directory;
    std::string filepath;
    bool isLoaded = false;
    
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh);
};