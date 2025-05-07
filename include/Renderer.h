#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

class Scene;
class Camera;

enum class RenderMode {
    Solid,
    Wireframe,
    Textured
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
    
private:
    RenderMode renderMode = RenderMode::Solid;
    LightingModel lightingModel = LightingModel::Phong;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    bool depthTestEnabled = true;
};