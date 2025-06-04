#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

class Scene;
class Camera;

enum class RenderMode {
    Solid,
    Wireframe,
    Deferred
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
    
private:
    RenderMode renderMode = RenderMode::Solid;
    LightingModel lightingModel = LightingModel::Phong;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    bool depthTestEnabled = true;
    
    // G-buffer objects for deferred rendering
    bool deferredSetupComplete = false;
    unsigned int gBuffer = 0;
    unsigned int gPosition = 0;
    unsigned int gNormal = 0;
    unsigned int gAlbedoSpec = 0;
    unsigned int gDepth = 0;
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    
    // Create a quad for rendering the final lighting pass
    void SetupScreenQuad();
};