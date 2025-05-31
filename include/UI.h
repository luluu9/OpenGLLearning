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
    void ShowShaderEditor(bool show) { showShaderEditor = show; }
    bool IsShaderEditorShown() const { return showShaderEditor; }
    
    // Object properties panel
    void ShowObjectProperties(bool show) { showObjectProperties = show; }
    bool IsObjectPropertiesShown() const { return showObjectProperties; }
    
    // Scene settings panel
    void ShowSceneSettings(bool show) { showSceneSettings = show; }
    bool IsSceneSettingsShown() const { return showSceneSettings; }
    
    // Set callback for when a new scene is loaded
    void SetOnSceneLoadCallback(std::function<void(const std::string&)> callback) { onSceneLoad = callback; }
    
    // Set callback for when a scene is saved
    void SetOnSceneSaveCallback(std::function<void(const std::string&)> callback) { onSceneSave = callback; }
    
    // Set callback for when a new object is added
    void SetOnAddObjectCallback(std::function<void(const std::string&)> callback) { onAddObject = callback; }
    
    // Set callbacks for when shader code is compiled
    void SetOnCompileShaderCallback(std::function<bool(Shader*, const std::string&, const std::string&)> callback) { onCompileShader = callback; }
    
    // Set callback for when a custom model is imported
    void SetOnImportModelCallback(std::function<void(const std::string&)> callback) { onImportModel = callback; }
    
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
    std::string OpenModelDialog();
    
    // Helper method to import a 3D model
    void ImportCustomModel(const std::string& filepath);
    
    // UI state
    bool showDemoWindow = false;
    bool showShaderEditor = true;
    bool showObjectProperties = true;
    bool showSceneSettings = true;
    bool showPerformanceOverlay = true;

    // UI settings
    int panelMargin = 30; 
    
    // References
    GLFWwindow* window = nullptr;
    SceneObject* selectedObject = nullptr;
    
    // Shader editor state
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    bool shaderModified = false;
    bool compilationSuccessful = false;
    std::string compilationMessage;
    
    // Callbacks
    std::function<void(const std::string&)> onSceneLoad;
    std::function<void(const std::string&)> onSceneSave;
    std::function<void(const std::string&)> onAddObject;
    std::function<bool(Shader*, const std::string&, const std::string&)> onCompileShader;
    std::function<void(const std::string&)> onImportModel;
    
    // Frame timing
    float frameTimes[100] = { 0 };
    int frameTimeIndex = 0;
    float frameTimeAccumulator = 0.0f;
    int frameCount = 0;
    float averageFrameTime = 0.0f;
    float lastFrameTime = 0.0f;
};