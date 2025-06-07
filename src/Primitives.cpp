#include "Primitives.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

std::unique_ptr<Mesh> Primitives::CreateCube(float size)
{
    float halfSize = size / 2.0f;
    
    // Vertex positions
    std::vector<Vertex> vertices = {
        // Front face
        { glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // 0
        { glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // 1
        { glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // 2
        { glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // 3
        
        // Back face
        { glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) }, // 4
        { glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) }, // 5
        { glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) }, // 6
        { glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) }, // 7
        
        // Top face
        { glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) }, // 8
        { glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // 9
        { glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // 10
        { glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }, // 11
        
        // Bottom face
        { glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // 12
        { glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // 13
        { glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }, // 14
        { glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) }, // 15
        
        // Right face
        { glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // 16
        { glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) }, // 17
        { glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }, // 18
        { glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // 19
        
        // Left face
        { glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // 20
        { glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // 21
        { glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) }, // 22
        { glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }  // 23
    };
    
    // Indices
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        
        // Back face
        4, 5, 6, 6, 7, 4,
        
        // Top face
        8, 9, 10, 10, 11, 8,
        
        // Bottom face
        12, 13, 14, 14, 15, 12,
        
        // Right face
        16, 17, 18, 18, 19, 16,
        
        // Left face
        20, 21, 22, 22, 23, 20
    };
    
    auto mesh = std::make_unique<Mesh>();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    
    return mesh;
}

std::unique_ptr<Mesh> Primitives::CreateSphere(float radius, unsigned int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    const unsigned int X_SEGMENTS = segments;
    const unsigned int Y_SEGMENTS = segments;
    
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            
            float xPos = radius * std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
            float yPos = radius * std::cos(ySegment * glm::pi<float>());
            float zPos = radius * std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
            
            glm::vec3 position(xPos, yPos, zPos);
            glm::vec3 normal = glm::normalize(position);
            glm::vec2 texCoord(xSegment, ySegment);
            
            vertices.push_back({position, normal, texCoord});
        }
    }
    
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x)
        {
            unsigned int topLeft = y * (X_SEGMENTS + 1) + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (y + 1) * (X_SEGMENTS + 1) + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    auto mesh = std::make_unique<Mesh>();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    
    return mesh;
}

std::unique_ptr<Mesh> Primitives::CreatePlane(float width, float height)
{
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    std::vector<Vertex> vertices = {
        { glm::vec3(-halfWidth, 0.0f, -halfHeight), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3( halfWidth, 0.0f, -halfHeight), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3( halfWidth, 0.0f,  halfHeight), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-halfWidth, 0.0f,  halfHeight), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) }
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0
    };
    
    auto mesh = std::make_unique<Mesh>();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    
    return mesh;
}

std::unique_ptr<Mesh> Primitives::CreateCylinder(float radius, float height, unsigned int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Calculate vertices and indices for the sides of the cylinder
    float halfHeight = height / 2.0f;
    float angleStep = 2.0f * glm::pi<float>() / static_cast<float>(segments);
    
    // Add center vertices for top and bottom caps
    vertices.push_back({ glm::vec3(0.0f, halfHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f) });
    unsigned int topCenterIndex = 0;
    
    vertices.push_back({ glm::vec3(0.0f, -halfHeight, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f) });
    unsigned int bottomCenterIndex = 1;
    
    // Generate vertices for the sides, top and bottom
    for (unsigned int i = 0; i <= segments; ++i)
    {
        float angle = i * angleStep;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        
        // Top rim vertex
        glm::vec3 topPosition(x, halfHeight, z);
        glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
        glm::vec2 topTexCoord(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle));
        vertices.push_back({ topPosition, topNormal, topTexCoord });
        
        // Bottom rim vertex
        glm::vec3 bottomPosition(x, -halfHeight, z);
        glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
        glm::vec2 bottomTexCoord(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle));
        vertices.push_back({ bottomPosition, bottomNormal, bottomTexCoord });
        
        // Side vertices (duplicate positions but different normals for proper lighting)
        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, 0.0f, z));
        glm::vec2 sideTexCoord(static_cast<float>(i) / segments, 1.0f);
        vertices.push_back({ topPosition, sideNormal, sideTexCoord });
        
        sideTexCoord = glm::vec2(static_cast<float>(i) / segments, 0.0f);
        vertices.push_back({ bottomPosition, sideNormal, sideTexCoord });
    }
    
    // Generate indices for the top and bottom caps
    for (unsigned int i = 0; i < segments; ++i)
    {
        // Top cap triangles
        indices.push_back(topCenterIndex);
        indices.push_back(2 + (i * 4));
        indices.push_back(2 + (((i + 1) % segments) * 4));
        
        // Bottom cap triangles
        indices.push_back(bottomCenterIndex);
        indices.push_back(3 + (((i + 1) % segments) * 4));
        indices.push_back(3 + (i * 4));
    }
    
    // Generate indices for the sides
    for (unsigned int i = 0; i < segments; ++i)
    {
        unsigned int current = 4 + (i * 4);
        unsigned int next = 4 + (((i + 1) % segments) * 4);
        
        // First triangle
        indices.push_back(current);
        indices.push_back(current + 1);
        indices.push_back(next);
        
        // Second triangle
        indices.push_back(next);
        indices.push_back(current + 1);
        indices.push_back(next + 1);
    }
    
    auto mesh = std::make_unique<Mesh>();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    
    return mesh;
}

std::unique_ptr<Mesh> Primitives::CreateCone(float radius, float height, unsigned int segments)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfHeight = height / 2.0f;
    float angleStep = 2.0f * glm::pi<float>() / static_cast<float>(segments);
    
    // Add the tip vertex
    vertices.push_back({ glm::vec3(0.0f, halfHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 0.5f) });
    unsigned int tipIndex = 0;
    
    // Add center of base
    vertices.push_back({ glm::vec3(0.0f, -halfHeight, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f, 0.5f) });
    unsigned int baseIndex = 1;
    
    // Generate vertices for the base and sides
    for (unsigned int i = 0; i <= segments; ++i)
    {
        float angle = i * angleStep;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        
        // Base vertex
        glm::vec3 basePosition(x, -halfHeight, z);
        glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
        glm::vec2 baseTexCoord(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle));
        vertices.push_back({ basePosition, baseNormal, baseTexCoord });
        
        // Side vertex (same position, but normal pointing outward)
        // Calculate normal for smooth shading
        glm::vec3 sideNormal = glm::normalize(glm::vec3(x, radius * 0.5f, z));
        glm::vec2 sideTexCoord(static_cast<float>(i) / segments, 0.0f);
        vertices.push_back({ basePosition, sideNormal, sideTexCoord });
    }
    
    // Generate indices for the base
    for (unsigned int i = 0; i < segments; ++i)
    {
        indices.push_back(baseIndex);
        indices.push_back(2 + (i * 2));
        indices.push_back(2 + (((i + 1) % segments) * 2));
    }
    
    // Generate indices for the sides
    for (unsigned int i = 0; i < segments; ++i)
    {
        indices.push_back(tipIndex);
        indices.push_back(3 + (((i + 1) % segments) * 2));
        indices.push_back(3 + (i * 2));
    }
    
    auto mesh = std::make_unique<Mesh>();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    
    return mesh;
}

std::unique_ptr<SceneObject> Primitives::CreatePrimitiveObject(const std::string& type, const std::string& name)
{
    auto object = std::make_unique<SceneObject>(name);
    std::unique_ptr<Mesh> mesh;
    
    if (type == "Cube")
    {
        mesh = CreateCube();
    }
    else if (type == "Sphere")
    {
        mesh = CreateSphere();
    }
    else if (type == "Plane")
    {
        mesh = CreatePlane();
    }
    else if (type == "Cylinder")
    {
        mesh = CreateCylinder();
    }
    else if (type == "Cone")
    {
        mesh = CreateCone();
    }
    else
    {
        std::cerr << "Unknown primitive type: " << type << std::endl;
        return nullptr;
    }

    object->SetMesh(std::move(mesh));

    ResourceManager* resourceManager = ResourceManager::GetInstance();
    Shader* shader = resourceManager->GetShader("default");
    
    if (!shader)
    {
        shader = resourceManager->LoadShaderFromFile("default", 
            "resources/shaders/default.vert", 
            "resources/shaders/default.frag");
        
        if (!shader)
        {
            std::cerr << "Failed to load default shaders for " << name << std::endl;
            return nullptr;
        }
    }
    
    object->SetShader(shader);
    SetupDefaultMaterial(object.get());
    
    return object;
}

void Primitives::SetupDefaultMaterial(SceneObject* object)
{
    if (!object) return;
    
    Material material;
    material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    material.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 32.0f;
    
    object->SetMaterial(material);
}