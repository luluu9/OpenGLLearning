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
    
    void SetRenderMode(RenderMode mode) { m_RenderMode = mode; }
    RenderMode GetRenderMode() const { return m_RenderMode; }
    
    void SetLightingModel(LightingModel model) { m_LightingModel = model; }
    LightingModel GetLightingModel() const { return m_LightingModel; }
    
    void SetClearColor(const glm::vec4& color) { m_ClearColor = color; }
    glm::vec4 GetClearColor() const { return m_ClearColor; }
    
    void EnableDepthTest(bool enable) { m_DepthTestEnabled = enable; }
    bool IsDepthTestEnabled() const { return m_DepthTestEnabled; }
    
private:
    RenderMode m_RenderMode = RenderMode::Solid;
    LightingModel m_LightingModel = LightingModel::Phong;
    glm::vec4 m_ClearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    bool m_DepthTestEnabled = true;
};