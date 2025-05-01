#include "SceneObject.h"
#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

SceneObject::SceneObject(const std::string& name)
    : m_Name(name)
{
}

SceneObject::~SceneObject() = default;

void SceneObject::Update(float deltaTime)
{
    // This base implementation doesn't do any animation or time-based updates
    // Derived classes can override to add custom behavior
}

void SceneObject::Draw()
{
    if (!m_Visible || !m_Mesh || !m_Shader)
        return;
        
    m_Mesh->Draw();
}

void SceneObject::SetPosition(const glm::vec3& position)
{
    m_Position = position;
    m_TransformDirty = true;
}

void SceneObject::SetRotation(const glm::vec3& rotation)
{
    m_Rotation = rotation;
    m_TransformDirty = true;
}

void SceneObject::SetScale(const glm::vec3& scale)
{
    m_Scale = scale;
    m_TransformDirty = true;
}

glm::mat4 SceneObject::GetTransform()
{
    if (m_TransformDirty)
    {
        // Calculate the model matrix from position, rotation and scale
        m_Transform = glm::mat4(1.0f);
        
        // Apply translation
        m_Transform = glm::translate(m_Transform, m_Position);
        
        // Apply rotation (Euler angles in degrees, converted to radians)
        m_Transform = m_Transform * glm::eulerAngleXYZ(
            glm::radians(m_Rotation.x),
            glm::radians(m_Rotation.y),
            glm::radians(m_Rotation.z)
        );
        
        // Apply scale
        m_Transform = glm::scale(m_Transform, m_Scale);
        
        m_TransformDirty = false;
    }
    
    return m_Transform;
}

void SceneObject::SetTransform(const glm::mat4& transform)
{
    m_Transform = transform;
    
    // Extract position, rotation, and scale from the matrix for editing
    // This is a simplification and may not be perfect for all cases
    
    // Extract position
    m_Position.x = m_Transform[3][0];
    m_Position.y = m_Transform[3][1];
    m_Position.z = m_Transform[3][2];
    
    // Extract scale
    m_Scale.x = glm::length(glm::vec3(m_Transform[0][0], m_Transform[0][1], m_Transform[0][2]));
    m_Scale.y = glm::length(glm::vec3(m_Transform[1][0], m_Transform[1][1], m_Transform[1][2]));
    m_Scale.z = glm::length(glm::vec3(m_Transform[2][0], m_Transform[2][1], m_Transform[2][2]));
    
    // To extract rotation correctly would require more complex matrix decomposition
    // For simplicity, we'll leave rotation as is
    
    m_TransformDirty = false; // The transform is now up to date
}