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
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }
    
    // Extract directory path
    directory = path.substr(0, path.find_last_of('/'));
    if (directory.empty())
        directory = path.substr(0, path.find_last_of('\\'));
    
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
    
    // Then process all child nodes recursively
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        
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
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    auto meshObj = std::make_unique<Mesh>();
    meshObj->SetVertices(vertices);
    meshObj->SetIndices(indices);
    
    return meshObj;
}