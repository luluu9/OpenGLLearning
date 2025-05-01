#include "Mesh.h"
#include <iostream>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    DeleteBuffers();
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices)
{
    m_Vertices = vertices;
    SetupMesh();
}

void Mesh::SetIndices(const std::vector<unsigned int>& indices)
{
    m_Indices = indices;
    SetupMesh();
}

void Mesh::Draw() const
{
    if (m_VAO == 0 || m_Vertices.empty())
        return;
    
    glBindVertexArray(m_VAO);
    
    if (!m_Indices.empty())
    {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_Vertices.size()));
    }
    
    glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
    // Delete existing buffers if they exist
    DeleteBuffers();
    
    // Only setup if we have vertices
    if (m_Vertices.empty())
        return;
    
    // Create buffers/arrays
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    
    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);
    
    // Load index data if we have indices
    if (!m_Indices.empty())
    {
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
    }
    
    // Set vertex attribute pointers
    
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // Vertex texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    glBindVertexArray(0);
}

void Mesh::DeleteBuffers()
{
    if (m_EBO != 0)
    {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    
    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}