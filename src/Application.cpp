#include "Application.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& title, int width, int height)
    : m_Title(title), m_Width(width), m_Height(height)
{
    s_Instance = this;
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize Renderer
    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Initialize())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    // Create camera
    m_Camera = std::make_unique<Camera>(45.0f, static_cast<float>(m_Width) / static_cast<float>(m_Height), 0.1f, 1000.0f);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));

    // Initialize Scene
    m_Scene = std::make_unique<Scene>();

    // Initialize UI
    m_UI = std::make_unique<UI>();
    if (!m_UI->Initialize(m_Window))
    {
        std::cerr << "Failed to initialize UI" << std::endl;
        return false;
    }

    // Setup resize callback
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Application* app = Application::GetInstance();
        if (app && app->GetCamera()) {
            app->GetCamera()->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        }
    });

    m_Running = true;
    return true;
}

void Application::Run()
{
    m_LastFrameTime = static_cast<float>(glfwGetTime());

    // Main loop
    while (m_Running && !glfwWindowShouldClose(m_Window))
    {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        m_DeltaTime = currentTime - m_LastFrameTime;
        m_LastFrameTime = currentTime;

        // Process input
        ProcessInput();

        // Update the scene
        Update();

        // Render the scene
        Render();

        // Swap front and back buffers
        glfwSwapBuffers(m_Window);

        // Poll for and process events
        glfwPollEvents();
    }
}

void Application::ProcessInput()
{
    // Check if ESC was pressed
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_Running = false;

    // Process camera input
    if (m_Camera)
        m_Camera->ProcessInput(m_Window, m_DeltaTime);
}

void Application::Update()
{
    if (m_Scene)
        m_Scene->Update(m_DeltaTime);

    if (m_UI)
        m_UI->Update();
}

void Application::Render()
{
    if (m_Renderer && m_Scene && m_Camera)
    {
        m_Renderer->BeginFrame();
        m_Renderer->Render(m_Scene.get(), m_Camera.get());
        
        if (m_UI)
            m_UI->Render();
            
        m_Renderer->EndFrame();
    }
}

void Application::Shutdown()
{
    if (m_UI)
        m_UI->Shutdown();

    if (m_Renderer)
        m_Renderer->Shutdown();

    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    glfwTerminate();
    s_Instance = nullptr;
}