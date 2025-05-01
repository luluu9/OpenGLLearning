#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "SceneObject.h"
#include "Shader.h"
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
    
    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    return true;
}

void Renderer::Shutdown()
{
    // No specific cleanup needed for now
}

void Renderer::BeginFrame()
{
    // Clear the screen
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set depth test state
    if (m_DepthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
        
    // Set render mode
    switch (m_RenderMode)
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
        shader->SetInt("lightingModel", static_cast<int>(m_LightingModel));
        
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
    }
}

void Renderer::EndFrame()
{
    // Reset to default rendering state
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}