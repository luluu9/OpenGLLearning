#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "SceneObject.h"
#include "Shader.h"
#include "ResourceManager.h"
#include <iostream>

Renderer::Renderer() = default;

Renderer::~Renderer() = default;

bool Renderer::Initialize()
{
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Enable depth testing by default
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    
    return true;
}

void Renderer::Shutdown()
{
    // No specific cleanup needed for now
}

void Renderer::BeginFrame()
{
    // Clear the screen
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set depth test state
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
        
    // Set render mode
    switch (renderMode)
    {
    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RenderMode::Solid:
    case RenderMode::Textured:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

void Renderer::Render(Scene* scene, Camera* camera)
{
    if (!scene || !camera)
        return;

    // Get the view and projection matrices from the camera
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    
    // Render each object in the scene
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible())
            continue;
            
        // Get the shader for this object
        Shader* shader = object->GetShader();
        if (!shader)
            continue;
            
        // Activate shader
        shader->Use();
        
        // Update lighting model in shader
        shader->SetInt("lightingModel", static_cast<int>(lightingModel));
        
        // Update lighting parameters
        for (const auto& light : scene->GetLights())
        {
            shader->SetVec3("lightPos", light.position);
            shader->SetVec3("lightColor", light.color);
            shader->SetFloat("lightIntensity", light.intensity);
        }
        
        // Set camera position for specular calculations
        shader->SetVec3("viewPos", camera->GetPosition());
        
        // Set material properties
        shader->SetVec3("material.ambient", object->GetMaterial().ambient);
        shader->SetVec3("material.diffuse", object->GetMaterial().diffuse);
        shader->SetVec3("material.specular", object->GetMaterial().specular);
        shader->SetFloat("material.shininess", object->GetMaterial().shininess);
        
        // Update transform matrices
        glm::mat4 modelMatrix = object->GetTransform();
        shader->SetMat4("model", modelMatrix);
        shader->SetMat4("view", viewMatrix);
        shader->SetMat4("projection", projectionMatrix);
        
        // Draw the object
        object->Draw();

        if (object->IsHighlighted())
        {
            Shader* highlightShader = ResourceManager::GetInstance()->GetShader("highlight");
            if (highlightShader)
            {
                highlightShader->Use();
                
                // Update transform matrices
                glm::mat4 modelMatrix = object->GetTransform();
                highlightShader->SetMat4("model", modelMatrix);
                highlightShader->SetMat4("view", viewMatrix);
                highlightShader->SetMat4("projection", projectionMatrix);
                
                // Draw the highlight
                object->DrawHighlight();
            }
        }
    }
}

void Renderer::EndFrame()
{
    // Reset to default rendering state
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Make sure depth test is enabled - important for 3D rendering
    glEnable(GL_DEPTH_TEST);
}

// Add a method to prepare for UI rendering
void Renderer::PrepareForUIRendering()
{
    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);
    
    // Enable blending for UI elements
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable face culling for UI
    glDisable(GL_CULL_FACE);
}

// Add a method to restore OpenGL state after UI rendering
void Renderer::RestoreAfterUIRendering()
{
    // Restore depth test state
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    // Restore polygon mode
    switch (renderMode)
    {
    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RenderMode::Solid:
    case RenderMode::Textured:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}