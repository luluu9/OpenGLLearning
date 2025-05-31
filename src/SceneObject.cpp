#include "SceneObject.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

SceneObject::SceneObject(const std::string& name)
    : name(name)
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
    if (!visible || !shader)
        return;

    if (mesh)
    {
        mesh->Draw();
    }
    else if (model)
    {
        model->Draw();
    }
}

void SceneObject::SetPosition(const glm::vec3& newPosition)
{
    position = newPosition;
    transformDirty = true;
}

void SceneObject::SetRotation(const glm::vec3& newRotation)
{
    rotation = newRotation;
    transformDirty = true;
}

void SceneObject::SetScale(const glm::vec3& newScale)
{
    scale = newScale;
    transformDirty = true;
}

glm::mat4 SceneObject::GetTransform()
{
    if (transformDirty)
    {
        // Calculate the model matrix from position, rotation and scale
        transform = glm::mat4(1.0f);
        
        // Apply translation
        transform = glm::translate(transform, position);
        
        // Apply rotation (Euler angles in degrees, converted to radians)
        transform = transform * glm::eulerAngleXYZ(
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        );
        
        // Apply scale
        transform = glm::scale(transform, scale);
        
        transformDirty = false;
    }
    
    return transform;
}

void SceneObject::SetTransform(const glm::mat4& newTransform)
{
    transform = newTransform;
    
    // Extract position, rotation, and scale from the matrix for editing
    // This is a simplification and may not be perfect for all cases
    
    // Extract position
    position.x = transform[3][0];
    position.y = transform[3][1];
    position.z = transform[3][2];
    
    // Extract scale
    scale.x = glm::length(glm::vec3(transform[0][0], transform[0][1], transform[0][2]));
    scale.y = glm::length(glm::vec3(transform[1][0], transform[1][1], transform[1][2]));
    scale.z = glm::length(glm::vec3(transform[2][0], transform[2][1], transform[2][2]));
    
    // To extract rotation correctly would require more complex matrix decomposition
    // For simplicity, we'll leave rotation as is
    
    transformDirty = false; // The transform is now up to date
}