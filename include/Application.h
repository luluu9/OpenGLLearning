#pragma once

#include <memory>
#include <string>
#include "Renderer.h"
#include "Scene.h"
#include "UI.h"
#include "Camera.h"

class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();

    void Run();
    bool Initialize();
    void Shutdown();

    static Application* GetInstance() { return s_Instance; }
    
    GLFWwindow* GetWindow() const { return m_Window; }
    Renderer* GetRenderer() const { return m_Renderer.get(); }
    Scene* GetScene() const { return m_Scene.get(); }
    UI* GetUI() const { return m_UI.get(); }
    Camera* GetCamera() const { return m_Camera.get(); }
    
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    float GetDeltaTime() const { return m_DeltaTime; }
    
private:
    void ProcessInput();
    void Update();
    void Render();
    
    static Application* s_Instance;
    
    std::string m_Title;
    int m_Width;
    int m_Height;
    GLFWwindow* m_Window = nullptr;
    
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Scene> m_Scene;
    std::unique_ptr<UI> m_UI;
    std::unique_ptr<Camera> m_Camera;
    
    float m_LastFrameTime = 0.0f;
    float m_DeltaTime = 0.0f;
    bool m_Running = false;
};