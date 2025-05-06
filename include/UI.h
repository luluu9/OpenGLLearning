#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string>
#include <functional>

class Scene;
class Renderer;
class Shader;
class SceneObject;

class UI {
public:
    UI();
    ~UI();
    
    bool Initialize(GLFWwindow* window);
    void Shutdown();
    
    void Update();
    void Render();
    
    // Shader editing
    void ShowShaderEditor(bool show) { m_ShowShaderEditor = show; }
    bool IsShaderEditorShown() const { return m_ShowShaderEditor; }
    
    // Object properties panel
    void ShowObjectProperties(bool show) { m_ShowObjectProperties = show; }
    bool IsObjectPropertiesShown() const { return m_ShowObjectProperties; }
    
    // Scene settings panel
    void ShowSceneSettings(bool show) { m_ShowSceneSettings = show; }
    bool IsSceneSettingsShown() const { return m_ShowSceneSettings; }
    
    // Set callback for when a new scene is loaded
    void SetOnSceneLoadCallback(std::function<void(const std::string&)> callback) { m_OnSceneLoad = callback; }
    
    // Set callback for when a scene is saved
    void SetOnSceneSaveCallback(std::function<void(const std::string&)> callback) { m_OnSceneSave = callback; }
    
    // Set callback for when a new object is added
    void SetOnAddObjectCallback(std::function<void(const std::string&)> callback) { m_OnAddObject = callback; }
    
    // Set callbacks for when shader code is compiled
    void SetOnCompileShaderCallback(std::function<bool(Shader*, const std::string&, const std::string&)> callback) { m_OnCompileShader = callback; }
    
    bool IsCapturingKeyboard() const;
    bool IsCapturingMouse() const;
    
private:
    // ImGui windows
    void RenderMainMenuBar();
    void RenderShaderEditor();
    void RenderObjectProperties();
    void RenderSceneSettings();
    void RenderPerformanceOverlay();
    
    // File dialogs
    std::string OpenFileDialog(const char* filter);
    std::string SaveFileDialog(const char* filter);
    
    // UI state
    bool m_ShowDemoWindow = false;
    bool m_ShowShaderEditor = true;
    bool m_ShowObjectProperties = true;
    bool m_ShowSceneSettings = true;
    bool m_ShowPerformanceOverlay = true;

    // UI settings
    int panelMargin = 30; 
    
    // References
    GLFWwindow* m_Window = nullptr;
    SceneObject* m_SelectedObject = nullptr;
    
    // Shader editor state
    std::string m_VertexShaderSource;
    std::string m_FragmentShaderSource;
    bool m_ShaderModified = false;
    
    // Callbacks
    std::function<void(const std::string&)> m_OnSceneLoad;
    std::function<void(const std::string&)> m_OnSceneSave;
    std::function<void(const std::string&)> m_OnAddObject;
    std::function<bool(Shader*, const std::string&, const std::string&)> m_OnCompileShader;
    
    // Frame timing
    float m_FrameTimes[100] = { 0 };
    int m_FrameTimeIndex = 0;
    float m_FrameTimeAccumulator = 0.0f;
    int m_FrameCount = 0;
    float m_AverageFrameTime = 0.0f;
    float m_LastFrameTime = 0.0f;
};