#pragma once

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    enum class RenderMode {
        TRIANGLES,
        PATCHES
    };
    
    Mesh();
    ~Mesh();
    
    void SetVertices(const std::vector<Vertex>& vertices);
    void SetIndices(const std::vector<unsigned int>& indices);
    
    void Draw(RenderMode mode = RenderMode::TRIANGLES) const;
    
    // Accessors
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }
    
private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    
    void SetupMesh();
    void DeleteBuffers();
};