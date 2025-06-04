#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "SceneObject.h"
#include "Shader.h"
#include "ResourceManager.h"
#include <iostream>
#include <GLFW/glfw3.h>

Renderer::Renderer() = default;

Renderer::~Renderer()
{
    CleanupDeferredRendering();
}

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
    
    // Setup screen quad for deferred rendering
    SetupScreenQuad();
    
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
    case RenderMode::Deferred:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

void Renderer::Render(Scene* scene, Camera* camera)
{
    if (!scene || !camera)
        return;

    if (renderMode == RenderMode::Deferred) {
        Shader* gBufferShader = ResourceManager::GetInstance()->GetShader("Laboratory_IV/gbuffer");
        Shader* lightingShader = ResourceManager::GetInstance()->GetShader("Laboratory_IV/deferred_lighting");
        
        if (!gBufferShader) {
            std::cerr << "Loading gbuffer shader..." << std::endl;
            gBufferShader = ResourceManager::GetInstance()->LoadShaderFromFile(
                "Laboratory_IV/gbuffer", 
                "resources/shaders/Laboratory_IV/gbuffer.vert", 
                "resources/shaders/Laboratory_IV/gbuffer.frag"
            );
        }

        if (!lightingShader) {
            std::cerr << "Loading deferred_lighting shader..." << std::endl;
            lightingShader = ResourceManager::GetInstance()->LoadShaderFromFile(
                "Laboratory_IV/deferred_lighting", 
                "resources/shaders/Laboratory_IV/deferred_lighting.vert", 
                "resources/shaders/Laboratory_IV/deferred_lighting.frag"
            );
        }
            
        if (!gBufferShader || !lightingShader) {
            std::cerr << "Failed to load deferred shaders, continue using standard rendering." << std::endl;
        } else {
            RenderDeferred(scene, camera);
            return;
        }
    }

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
        object->DrawHighlight(camera);
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
    case RenderMode::Deferred:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

// Setup screen-aligned quad for the deferred lighting pass
void Renderer::SetupScreenQuad()
{
    float quadVertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); // Fixed: removed & operator
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Unbind VAO to prevent accidental modifications
    glBindVertexArray(0);
}

void Renderer::SetupDeferredRendering()
{
    if (deferredSetupComplete)
        return;
    
    // Clean up any existing resources first
    CleanupDeferredRendering();
    
    // Log setup for debugging
    std::cout << "Setting up deferred rendering..." << std::endl;
    
    // Get window size
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        std::cerr << "Error: No valid GLFW context for setup!" << std::endl;
        return;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    std::cout << "Framebuffer size: " << width << "x" << height << std::endl;
    
    // Create G-buffer
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    // Position buffer - RGB16F for HDR support
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    
    // Normal buffer - RGB16F for precise normals
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    
    // Albedo and specular intensity buffer - RGBA8 (RGB for albedo color, A for specular intensity)
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    
    // Tell OpenGL which attachments we'll use
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    
    // Create and attach a depth buffer
    glGenRenderbuffers(1, &gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);
      // Check if FBO is complete
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: G-buffer Framebuffer is not complete! Status: 0x" 
                  << std::hex << status << std::dec << std::endl;
        
        // Print more detailed error info
        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED: 
                std::cerr << "GL_FRAMEBUFFER_UNDEFINED" << std::endl; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: 
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: 
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: 
                std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: 
                std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl; break;
            default: 
                std::cerr << "Unknown framebuffer status error" << std::endl;
        }
        
        // Bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    } else {
        std::cout << "G-buffer setup complete!" << std::endl;
    }
    
    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    deferredSetupComplete = true;
}

void Renderer::CleanupDeferredRendering()
{
    if (!deferredSetupComplete)
        return;
    
    // Clean up G-buffer resources
    if (gBuffer) glDeleteFramebuffers(1, &gBuffer);
    if (gPosition) glDeleteTextures(1, &gPosition);
    if (gNormal) glDeleteTextures(1, &gNormal);
    if (gAlbedoSpec) glDeleteTextures(1, &gAlbedoSpec);
    if (gDepth) glDeleteRenderbuffers(1, &gDepth);
    
    // Clean up quad VAO/VBO
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
    
    gBuffer = 0;
    gPosition = 0;
    gNormal = 0;
    gAlbedoSpec = 0;
    gDepth = 0;
    quadVAO = 0;
    quadVBO = 0;
    
    deferredSetupComplete = false;
}

void Renderer::RenderDeferred(Scene* scene, Camera* camera)
{
    if (!scene || !camera)
        return;
    
    // THIS TOOK ME HOURS, BE CAREFUL
    glDisable(GL_BLEND);

    // Ensure G-buffer is set up
    if (!deferredSetupComplete) {
        std::cout << "Setting up deferred rendering for the first time..." << std::endl;
        SetupDeferredRendering();
        if (!deferredSetupComplete) {
            std::cerr << "Failed to set up deferred rendering!" << std::endl;
            return;
        }
    }
    
    // Get the view and projection matrices from the camera
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    
    // --- GEOMETRY PASS ---
    // Bind G-buffer
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Get the G-buffer shader
    Shader* gBufferShader = ResourceManager::GetInstance()->GetShader("Laboratory_IV/gbuffer");
    if (!gBufferShader) {
        std::cerr << "Error: G-buffer shader not found!" << std::endl;
        return;
    }
    gBufferShader->Use();
    
    // Render each object in the scene - only geometry
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible())
            continue;
        
        // Update transform matrices
        glm::mat4 modelMatrix = object->GetTransform();
        gBufferShader->SetMat4("model", modelMatrix);
        gBufferShader->SetMat4("view", viewMatrix);
        gBufferShader->SetMat4("projection", projectionMatrix);
        
        // Set material properties
        gBufferShader->SetVec3("material.ambient", object->GetMaterial().ambient);
        gBufferShader->SetVec3("material.diffuse", object->GetMaterial().diffuse);
        gBufferShader->SetVec3("material.specular", object->GetMaterial().specular);
        gBufferShader->SetFloat("material.shininess", object->GetMaterial().shininess);
        
        // Draw the object
        object->Draw();
    }

    // --- LIGHTING PASS ---
    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Get the deferred lighting shader
    Shader* lightingShader = ResourceManager::GetInstance()->GetShader("Laboratory_IV/deferred_lighting");
    if (!lightingShader) {
        std::cerr << "Error: Deferred lighting shader not found!" << std::endl;
        return;
    }
    lightingShader->Use();
    
    // Bind G-buffer textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    lightingShader->SetInt("gPosition", 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    lightingShader->SetInt("gNormal", 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    lightingShader->SetInt("gAlbedoSpec", 2);
    
    // Set lighting properties
    const auto& lights = scene->GetLights();
    if (!lights.empty()) {
        const auto& light = lights[0];  // Use first light for simplicity
        lightingShader->SetVec3("lightPos", light.position);
        lightingShader->SetVec3("lightColor", light.color);
        lightingShader->SetFloat("lightIntensity", light.intensity);
    }
    
    // Set camera position for specular calculations
    lightingShader->SetVec3("viewPos", camera->GetPosition());
    
    // Draw full-screen quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // --- RENDER HIGHLIGHTED OBJECTS ---
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible() || !object->IsHighlighted())
            continue;
            
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
            object->DrawHighlight(camera);
        }
    }
}