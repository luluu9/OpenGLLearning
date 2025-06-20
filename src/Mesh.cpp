#include "Mesh.h"
#include <iostream>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    DeleteBuffers();
}

void Mesh::SetVertices(const std::vector<Vertex>& newVertices)
{
    vertices = newVertices;
    SetupMesh();
}

void Mesh::SetIndices(const std::vector<unsigned int>& newIndices)
{
    indices = newIndices;
    SetupMesh();
}

void Mesh::Draw(RenderMode mode) const
{
    if (VAO == 0 || vertices.empty())
        return;
    
    glBindVertexArray(VAO);
    
    GLenum primitiveType = (mode == RenderMode::PATCHES) ? GL_PATCHES : GL_TRIANGLES;
    if (mode == RenderMode::PATCHES) {
        glPatchParameteri(GL_PATCH_VERTICES, 3);
    }
    
    if (!indices.empty())
    {
        glDrawElements(primitiveType, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(primitiveType, 0, static_cast<GLsizei>(vertices.size()));
    }
    
    glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
    DeleteBuffers();
    
    if (vertices.empty())
        return;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    if (!indices.empty())
    {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    glBindVertexArray(0);
}

void Mesh::DeleteBuffers()
{
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}