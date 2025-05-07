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

    static Application* GetInstance() { return instance; }
    
    GLFWwindow* GetWindow() const { return window; }
    Renderer* GetRenderer() const { return renderer.get(); }
    Scene* GetScene() const { return scene.get(); }
    UI* GetUI() const { return ui.get(); }
    Camera* GetCamera() const { return camera.get(); }
    
private:
    void ProcessInput();
    void Update();
    void Render();
    
    static Application* instance;
    
    std::string title;
    int width;
    int height;
    GLFWwindow* window = nullptr;
    
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<UI> ui;
    std::unique_ptr<Camera> camera;
    
    float lastFrameTime = 0.0f;
    float deltaTime = 0.0f;
    bool running = false;
};