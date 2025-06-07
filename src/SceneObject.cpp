#include "SceneObject.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "ResourceManager.h"

SceneObject::SceneObject(const std::string& name)
    : name(name)
{
}

SceneObject::~SceneObject() = default;

void SceneObject::Update(float deltaTime)
{
}

void SceneObject::Draw(Mesh::RenderMode mode)
{
    if (!visible || !shader)
        return;

    if (mesh)
    {
        mesh->Draw(mode);
    }
    else if (model)
    {
        model->Draw(mode);
    }
}

void SceneObject::DrawHighlight(Camera* camera)
{
    if (!visible || !highlighted)
        return;
    
    static float highlightPulse = 0.0f;
    highlightPulse += 0.05f; // Update pulse value for animation
    if (highlightPulse > 6.28f)
        highlightPulse = 0.0f;
    
    Shader* highlightShader = ResourceManager::GetInstance()->GetShader("highlight");
    if (!highlightShader)
        return;
    
    // Enable blending for transparent highlight effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use the highlight shader
    highlightShader->Use();
    
    // Set highlight color and pulse
    highlightShader->SetVec4("highlightColor", glm::vec4(1.0f, 0.6f, 0.0f, 0.3f)); 
    highlightShader->SetFloat("highlightPulse", highlightPulse);
    highlightShader->SetMat4("model", transform);
    highlightShader->SetMat4("view", camera->GetViewMatrix());
    highlightShader->SetMat4("projection", camera->GetProjectionMatrix());
    
    if (mesh)
    {
        mesh->Draw();
    }
    else if (model)
    {
        model->Draw();
    }

    glDisable(GL_BLEND);
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
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, position);
        transform = transform * glm::eulerAngleXYZ(
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        );
        
        transform = glm::scale(transform, scale);
        
        transformDirty = false;
    }
    
    return transform;
}
