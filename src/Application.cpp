#include "Application.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "ResourceManager.h"

Application* Application::instance = nullptr;

Application::Application(const std::string& title, int width, int height)
    : title(title), width(width), height(height)
{
    instance = this;
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize Renderer
    renderer = std::make_unique<Renderer>();
    if (!renderer->Initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    camera = std::make_unique<Camera>(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));

    scene = std::make_unique<Scene>();

    ui = std::make_unique<UI>();
    if (!ui->Initialize(window))
    {
        std::cerr << "Failed to initialize UI" << std::endl;
        return false;
    }
    
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    resourceManager->LoadAllShadersFromDirectory("resources/shaders");
    ui->RefreshShaderLibrary();

    glfwSetFramebufferSizeCallback(window, []([[maybe_unused]] GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Application* app = Application::GetInstance();
        if (app && app->GetCamera()) {
            if (height == 0 || width == 0) return;
            app->GetCamera()->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        }
    });

    running = true;
    return true;
}

void Application::Run()
{
    lastFrameTime = static_cast<float>(glfwGetTime());

    // Main loop
    while (running && !glfwWindowShouldClose(window))
    {
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        ProcessInput();
        Update();
        Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::ProcessInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        running = false;

    if (camera && ui)
    {
        camera->ProcessInput(window, deltaTime, ui->IsCapturingKeyboard(), ui->IsCapturingMouse());
    }
}

void Application::Update()
{
    if (scene)
        scene->Update(deltaTime);

    if (ui)
        ui->Update();
}

void Application::Render()
{
    if (renderer && scene && camera)
    {
        renderer->BeginFrame();
        renderer->Render(scene.get(), camera.get());
        
        if (ui)
        {
            renderer->PrepareForUIRendering();
            ui->Render();
            renderer->RestoreAfterUIRendering();
        }
            
        renderer->EndFrame();
    }
}

void Application::Shutdown()
{
    if (ui)
        ui->Shutdown();

    if (renderer)
        renderer->Shutdown();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
    instance = nullptr;
}