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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    
    SetupScreenQuad();
    
    return true;
}

void Renderer::Shutdown()
{
}

void Renderer::BeginFrame()
{
    currentTime = static_cast<float>(glfwGetTime());
    
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    switch (renderMode)
    {
    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RenderMode::Solid:
    case RenderMode::Deferred:
    case RenderMode::Tessellation:
    case RenderMode::TessellationWithWireframe:
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
        Shader* gBufferShader = ResourceManager::GetInstance()->GetShader("deferred/gbuffer");
        Shader* lightingShader = ResourceManager::GetInstance()->GetShader("deferred/deferred_lighting");
        
        if (!gBufferShader) {
            std::cerr << "Loading gbuffer shader..." << std::endl;
            gBufferShader = ResourceManager::GetInstance()->LoadShaderFromFile(
                "deferred/gbuffer", 
                "resources/shaders/deferred/gbuffer.vert", 
                "resources/shaders/deferred/gbuffer.frag"
            );
        }

        if (!lightingShader) {
            std::cerr << "Loading deferred_lighting shader..." << std::endl;
            lightingShader = ResourceManager::GetInstance()->LoadShaderFromFile(
                "deferred/deferred_lighting", 
                "resources/shaders/deferred/deferred_lighting.vert", 
                "resources/shaders/deferred/deferred_lighting.frag"
            );
        }
            
        if (!gBufferShader || !lightingShader) {
            std::cerr << "Failed to load deferred shaders, continue using standard rendering." << std::endl;
        } else {
            RenderDeferred(scene, camera);
            return;
        }
    }
    else if (renderMode == RenderMode::Tessellation || renderMode == RenderMode::TessellationWithWireframe) {
        Shader* tessShader = ResourceManager::GetInstance()->GetShader("tessellation");
        if (!tessShader) {
            std::cout << "Loading tessellation shaders..." << std::endl;            
            tessShader = ResourceManager::GetInstance()->LoadShaderWithTessellation(
                "tessellation",
                "resources/shaders/tessellation/tessellation.vert",
                "resources/shaders/tessellation/tessellation.frag",
                "resources/shaders/tessellation/tessellation.tesc",
                "resources/shaders/tessellation/tessellation.tese"
            );
        }
        
        if (!tessShader) {
            std::cerr << "Failed to load tessellation shaders, falling back to standard rendering." << std::endl;
        } else {
            RenderWithTessellation(scene, camera);
            return;
        }
    }

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    
    // Render each object in the scene
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible())
            continue;
            
        Shader* shader = object->GetShader();
        if (!shader)
            continue;
            
        shader->Use();
        shader->SetInt("lightingModel", static_cast<int>(lightingModel));

        const auto& lights = scene->GetLights();
        int numLights = static_cast<int>(lights.size());        shader->SetInt("numLights", numLights);
        
        // Pass each light to the shader
        for (int i = 0; i < numLights; i++) {
            const auto& light = lights[i];
            std::string prefix = "lights[" + std::to_string(i) + "].";
            
            shader->SetVec3(prefix + "position", light.position);
            shader->SetVec3(prefix + "color", light.color);
            shader->SetFloat(prefix + "intensity", light.intensity);
        }
        
        // Set time uniform for all shaders for animations
        shader->SetFloat("time", currentTime);
        
        shader->SetVec3("viewPos", camera->GetPosition());

        shader->SetVec3("material.ambient", object->GetMaterial().ambient);
        shader->SetVec3("material.diffuse", object->GetMaterial().diffuse);
        shader->SetVec3("material.specular", object->GetMaterial().specular);
        shader->SetFloat("material.shininess", object->GetMaterial().shininess);

        glm::mat4 modelMatrix = object->GetTransform();
        shader->SetMat4("model", modelMatrix);
        shader->SetMat4("view", viewMatrix);
        shader->SetMat4("projection", projectionMatrix);
        
        object->Draw();
        object->DrawHighlight(camera, currentTime);
    }
}

void Renderer::EndFrame()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::PrepareForUIRendering()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
}

void Renderer::RestoreAfterUIRendering()
{
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    switch (renderMode)
    {
    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RenderMode::Solid:
    case RenderMode::Deferred:
    case RenderMode::Tessellation:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Renderer::SetupDeferredRendering()
{
    if (deferredSetupComplete)
        return;
    
    CleanupDeferredRendering();
    
    std::cout << "Setting up deferred rendering..." << std::endl;
    
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        std::cerr << "Error: No valid GLFW context for setup!" << std::endl;
        return;
    }
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    std::cout << "Framebuffer size: " << width << "x" << height << std::endl;
    
    // G-buffer
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    // Position buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    
    // Normal buffer 
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    
    // Albedo and specular intensity buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    
    // Create and attach a depth buffer
    glGenRenderbuffers(1, &gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: G-buffer Framebuffer is not complete! Status: 0x" 
                  << std::hex << status << std::dec << std::endl;
   
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }
    std::cout << "G-buffer setup complete!" << std::endl;
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

void Renderer::RenderWithTessellation(Scene* scene, Camera* camera)
{
    if (!scene || !camera)
        return;
    
    if (!IsTessellationSupported()) {
        std::cerr << "Tessellation is not supported on this system. Falling back to standard rendering." << std::endl;
        // Fall back to standard rendering
        renderMode = RenderMode::Solid;
        Render(scene, camera);
        return;
    }
    
    if (renderMode == RenderMode::TessellationWithWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    Shader* tessellationShader = ResourceManager::GetInstance()->GetShader("tessellation");
    if (!tessellationShader) {
        std::cerr << "Error: Tessellation shader not found!" << std::endl;
        return;
    }
    tessellationShader->Use();
    
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error before setting uniforms: " << std::hex << err << std::dec << std::endl;
    }
      try {
        tessellationShader->SetFloat("tessLevelOuter", tessellationLevelOuter);
        tessellationShader->SetFloat("tessLevelInner", tessellationLevelInner);
        tessellationShader->SetFloat("displaceAmount", displacementAmount);
        
        // Set time uniform for animations
        tessellationShader->SetFloat("time", currentTime);
    } catch (const std::exception& e) {
        std::cerr << "Error setting tessellation parameters: " << e.what() << std::endl;
    }

    const auto& lights = scene->GetLights();
    try {
        if (!lights.empty()) {
            const auto& light = lights[0]; // Use first light for simplicity
            tessellationShader->SetVec3("lightPos", light.position);
            tessellationShader->SetVec3("lightColor", light.color);
            tessellationShader->SetFloat("lightIntensity", light.intensity);
        }
        tessellationShader->SetVec3("viewPos", camera->GetPosition());
    } catch (const std::exception& e) {
        std::cerr << "Error setting lighting parameters: " << e.what() << std::endl;
    }
    
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error after setting uniforms: " << std::hex << err << std::dec << std::endl;
    }
    glm::mat4 identityMatrix(1.0f);
    tessellationShader->SetMat4("view", camera->GetViewMatrix());
    tessellationShader->SetMat4("projection", camera->GetProjectionMatrix());
    
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error before simple rendering: " << std::hex << err << std::dec << std::endl;
    }
    
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible())
            continue;
        
        glm::mat4 modelMatrix = object->GetTransform();
        tessellationShader->SetMat4("model", modelMatrix);
        tessellationShader->SetVec3("material.ambient", object->GetMaterial().ambient);
        tessellationShader->SetVec3("material.diffuse", object->GetMaterial().diffuse);
        tessellationShader->SetVec3("material.specular", object->GetMaterial().specular);
        tessellationShader->SetFloat("material.shininess", object->GetMaterial().shininess);
          
        object->Draw(Mesh::RenderMode::PATCHES);
        while((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error after draw call: " << std::hex << err << std::dec << std::endl;
        }
                
    }
        
    // Render highlighted objects (using normal highlighting)
    Shader* highlightShader = ResourceManager::GetInstance()->GetShader("highlight");
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible() || !object->IsHighlighted())
            continue;
            
        if (highlightShader)
        {
            object->DrawHighlight(camera, currentTime);
        }
    }
    
    // Check for any OpenGL errors at the end
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error at end of tessellation rendering: " << std::hex << err << std::dec << std::endl;
    }
    
    // Restore polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    
    // --- GEOMETRY PASS ---
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Shader* gBufferShader = ResourceManager::GetInstance()->GetShader("deferred/gbuffer");
    if (!gBufferShader) {
        std::cerr << "Error: G-buffer shader not found!" << std::endl;
        return;
    }    gBufferShader->Use();
    
    // Render each object in the scene - only geometry
    for (auto& object : scene->GetObjects())
    {
        if (!object->IsVisible())
            continue;
        
        glm::mat4 modelMatrix = object->GetTransform();
        gBufferShader->SetMat4("model", modelMatrix);
        gBufferShader->SetMat4("view", viewMatrix);
        gBufferShader->SetMat4("projection", projectionMatrix);
        
        gBufferShader->SetVec3("material.ambient", object->GetMaterial().ambient);
        gBufferShader->SetVec3("material.diffuse", object->GetMaterial().diffuse);
        gBufferShader->SetVec3("material.specular", object->GetMaterial().specular);
        gBufferShader->SetFloat("material.shininess", object->GetMaterial().shininess);
        
        object->Draw();
    }

    // --- LIGHTING PASS ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Shader* lightingShader = ResourceManager::GetInstance()->GetShader("deferred/deferred_lighting");
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

    const auto& lights = scene->GetLights();
    int numLights = static_cast<int>(lights.size());
    lightingShader->SetInt("numLights", numLights);
    
    // Pass each light to the shader
    for (int i = 0; i < numLights; i++) {
        const auto& light = lights[i];
        std::string prefix = "lights[" + std::to_string(i) + "].";
        
        lightingShader->SetVec3(prefix + "position", light.position);
        lightingShader->SetVec3(prefix + "color", light.color);
        lightingShader->SetFloat(prefix + "intensity", light.intensity);
    }    
    lightingShader->SetVec3("viewPos", camera->GetPosition());
    lightingShader->SetFloat("time", currentTime);
    
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
            
            glm::mat4 modelMatrix = object->GetTransform();
            highlightShader->SetMat4("model", modelMatrix);
            highlightShader->SetMat4("view", viewMatrix);
            highlightShader->SetMat4("projection", projectionMatrix);
            
            object->DrawHighlight(camera, currentTime);
        }
    }
}

bool Renderer::IsTessellationSupported()
{
    int majorVersion, minorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    
    // Tessellation shaders are available in OpenGL 4.0 and higher
    bool supported = (majorVersion > 4) || (majorVersion == 4 && minorVersion >= 0);
    
    if (!supported) {
        std::cerr << "Tessellation not supported: OpenGL version " << majorVersion << "." << minorVersion 
                  << " detected. OpenGL 4.0+ required." << std::endl;
    }
    
    return supported;
}
