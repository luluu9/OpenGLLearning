#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

class Scene;
class Camera;
class Shader;

enum class RenderMode {
    Solid,
    Wireframe,
    Deferred,
    Tessellation,
    TessellationWithWireframe
};

enum class LightingModel {
    Flat,
    Phong
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Shutdown();
    
    void BeginFrame();
    void Render(Scene* scene, Camera* camera);
    void EndFrame();
    
    // Returns the current animation time
    float GetTime() const { return currentTime; }
    
    void PrepareForUIRendering();
    void RestoreAfterUIRendering();
    
    void SetRenderMode(RenderMode mode) { renderMode = mode; }
    RenderMode GetRenderMode() const { return renderMode; }
    
    void SetLightingModel(LightingModel model) { lightingModel = model; }
    LightingModel GetLightingModel() const { return lightingModel; }
    
    void SetClearColor(const glm::vec4& color) { clearColor = color; }
    glm::vec4 GetClearColor() const { return clearColor; }
    
    void EnableDepthTest(bool enable) { depthTestEnabled = enable; }
    bool IsDepthTestEnabled() const { return depthTestEnabled; }
    
    // Methods for deferred rendering
    void SetupDeferredRendering();
    void CleanupDeferredRendering();
    void RenderDeferred(Scene* scene, Camera* camera);
    
    // Methods for tessellation control
    void SetTessellationLevelOuter(float level) { tessellationLevelOuter = level; }
    float GetTessellationLevelOuter() const { return tessellationLevelOuter; }
    
    void SetTessellationLevelInner(float level) { tessellationLevelInner = level; }
    float GetTessellationLevelInner() const { return tessellationLevelInner; }
    
    void SetDisplacementAmount(float amount) { displacementAmount = amount; }
    float GetDisplacementAmount() const { return displacementAmount; }
    
    bool IsTessellationSupported();

private:
    // Rendering options
    RenderMode renderMode = RenderMode::Solid;
    LightingModel lightingModel = LightingModel::Phong;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    bool depthTestEnabled = true;
    float currentTime = 0.0f;
    
    // Tessellation control parameters
    float tessellationLevelOuter = 4.0f;
    float tessellationLevelInner = 4.0f;
    float displacementAmount = 0.3f;
    
    // Screen-space quad for deferred rendering
    bool deferredSetupComplete = false;
    unsigned int gBuffer = 0;
    unsigned int gPosition = 0;
    unsigned int gNormal = 0;
    unsigned int gAlbedoSpec = 0;
    unsigned int gDepth = 0;
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    
    void SetupScreenQuad();
    void RenderWithTessellation(Scene* scene, Camera* camera);
};