#include "Model.h"
#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

Model::Model() = default;

Model::~Model()
{
    // Clean up textures
    for (const auto& texture : loadedTextures)
    {
        if (texture.id != 0)
            glDeleteTextures(1, &texture.id);
    }
}

bool Model::LoadFromFile(const std::string& path)
{
    filepath = path;
    isLoaded = false;
    
    // Use Assimp to load the model
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);
    
    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }
    
    // Extract directory path
    directory = path.substr(0, path.find_last_of('/'));
    if (directory.empty())
        directory = path.substr(0, path.find_last_of('\\'));
    
    // Process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
    
    isLoaded = true;
    std::cout << "Successfully loaded model: " << path << std::endl;
    return true;
}

void Model::Draw(Mesh::RenderMode mode) const
{
    if (!isLoaded) {
        std::cerr << "Model not loaded: " << filepath << std::endl;
        return;
    }
        
    for (const auto& mesh : meshes)
    {
        if (mesh)
            mesh->Draw(mode);
    }
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process all the node's meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    
    // Then process all child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        
        // Position
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        
        // Normal
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        
        // Texture Coordinates
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        
        vertices.push_back(vertex);
    }
    
    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    // Create our mesh object
    auto meshObj = std::make_unique<Mesh>();
    meshObj->SetVertices(vertices);
    meshObj->SetIndices(indices);
    
    return meshObj;
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, int type, const std::string& typeName)
{
    // For now, return an empty vector since we're not handling textures
    return std::vector<Texture>();
}

unsigned int Model::TextureFromFile(const std::string& path)
{
    // Create a simple dummy texture for now
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Create a simple 1x1 white texture
    unsigned char data[] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    return textureID;
}