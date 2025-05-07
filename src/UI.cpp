#include "UI.h"
#include "Application.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Shader.h"
#include "Primitives.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

// Windows file dialog includes
#include <Windows.h>
#include <commdlg.h>
#include <direct.h>

UI::UI() = default;

UI::~UI()
{
    if (ImGui::GetCurrentContext())
        Shutdown();
}

bool UI::Initialize(GLFWwindow* window)
{
    if (!window)
        return false;
        
    m_Window = window;
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
        
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    return true;
}

void UI::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Calculate frame time
    float currentTime = static_cast<float>(glfwGetTime());
    float deltaTime = currentTime - m_LastFrameTime;
    m_LastFrameTime = currentTime;
    
    // Store frame time for performance monitoring
    m_FrameTimes[m_FrameTimeIndex] = deltaTime;
    m_FrameTimeIndex = (m_FrameTimeIndex + 1) % IM_ARRAYSIZE(m_FrameTimes);
    
    // Calculate average frame time
    m_FrameTimeAccumulator += deltaTime;
    m_FrameCount++;
    
    if (m_FrameTimeAccumulator >= 0.5f) // Update average every half second
    {
        m_AverageFrameTime = m_FrameTimeAccumulator / m_FrameCount;
        m_FrameTimeAccumulator = 0.0f;
        m_FrameCount = 0;
    }

    // Render ImGui components
    RenderMainMenuBar();
    
    if (m_ShowShaderEditor)
        RenderShaderEditor();
        
    if (m_ShowObjectProperties)
        RenderObjectProperties();
        
    if (m_ShowSceneSettings)
        RenderSceneSettings();
        
    if (m_ShowPerformanceOverlay)
        RenderPerformanceOverlay();
        
    if (m_ShowDemoWindow)
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}

void UI::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::RenderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", "Ctrl+N"))
            {
                Application* app = Application::GetInstance();
                if (app && app->GetScene())
                {
                    app->GetScene()->ClearObjects();
                    app->GetScene()->ClearLights();
                    app->GetScene()->AddDefaultLight();
                    
                    m_SelectedObject = nullptr;
                }
            }
            
            if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
            {
                // Show file dialog to open scene
                std::string filepath = OpenFileDialog("JSON Scene Files\0*.json\0All Files\0*.*\0");
                if (!filepath.empty() && m_OnSceneLoad)
                {
                    m_OnSceneLoad(filepath);
                    m_SelectedObject = nullptr; // Reset selected object
                }
            }
            
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                // Show file dialog to save scene
                std::string filepath = SaveFileDialog("JSON Scene Files\0*.json\0All Files\0*.*\0");
                if (!filepath.empty() && m_OnSceneSave)
                {
                    m_OnSceneSave(filepath);
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Shader Editor", nullptr, m_ShowShaderEditor))
                m_ShowShaderEditor = !m_ShowShaderEditor;
                
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Object Properties", nullptr, m_ShowObjectProperties))
                m_ShowObjectProperties = !m_ShowObjectProperties;
                
            if (ImGui::MenuItem("Scene Settings", nullptr, m_ShowSceneSettings))
                m_ShowSceneSettings = !m_ShowSceneSettings;
                
            if (ImGui::MenuItem("Performance Overlay", nullptr, m_ShowPerformanceOverlay))
                m_ShowPerformanceOverlay = !m_ShowPerformanceOverlay;
                
            ImGui::Separator();
            
            if (ImGui::MenuItem("ImGui Demo", nullptr, m_ShowDemoWindow))
                m_ShowDemoWindow = !m_ShowDemoWindow;
                
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Objects"))
        {
            if (ImGui::MenuItem("Add Cube"))
            {
                if (m_OnAddObject)
                    m_OnAddObject("Cube");
            }
            
            if (ImGui::MenuItem("Add Sphere"))
            {
                if (m_OnAddObject)
                    m_OnAddObject("Sphere");
            }
            
            if (ImGui::MenuItem("Add Plane"))
            {
                if (m_OnAddObject)
                    m_OnAddObject("Plane");
            }
            
            if (ImGui::MenuItem("Add Cylinder"))
            {
                if (m_OnAddObject)
                    m_OnAddObject("Cylinder");
            }
            
            if (ImGui::MenuItem("Add Cone"))
            {
                if (m_OnAddObject)
                    m_OnAddObject("Cone");
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Import Custom Model..."))
            {
                std::string filepath = OpenModelDialog();
                if (!filepath.empty())
                {
                    ImportCustomModel(filepath);
                }
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Rendering"))
        {
            Application* app = Application::GetInstance();
            if (app && app->GetRenderer())
            {
                Renderer* renderer = app->GetRenderer();
                
                if (ImGui::BeginMenu("Render Mode"))
                {
                    bool isSolid = renderer->GetRenderMode() == RenderMode::Solid;
                    if (ImGui::MenuItem("Solid", nullptr, isSolid))
                        renderer->SetRenderMode(RenderMode::Solid);
                        
                    bool isWireframe = renderer->GetRenderMode() == RenderMode::Wireframe;
                    if (ImGui::MenuItem("Wireframe", nullptr, isWireframe))
                        renderer->SetRenderMode(RenderMode::Wireframe);
                        
                    bool isTextured = renderer->GetRenderMode() == RenderMode::Textured;
                    if (ImGui::MenuItem("Textured", nullptr, isTextured))
                        renderer->SetRenderMode(RenderMode::Textured);
                        
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Lighting Model"))
                {
                    bool isFlat = renderer->GetLightingModel() == LightingModel::Flat;
                    if (ImGui::MenuItem("Flat", nullptr, isFlat))
                        renderer->SetLightingModel(LightingModel::Flat);
                        
                    bool isPhong = renderer->GetLightingModel() == LightingModel::Phong;
                    if (ImGui::MenuItem("Phong", nullptr, isPhong))
                        renderer->SetLightingModel(LightingModel::Phong);
                        
                    ImGui::EndMenu();
                }
                
                ImGui::Separator();
                
                bool depthTestEnabled = renderer->IsDepthTestEnabled();
                if (ImGui::MenuItem("Depth Test", nullptr, depthTestEnabled))
                    renderer->EnableDepthTest(!depthTestEnabled);
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void UI::RenderShaderEditor()
{
    // Set shader editor position at top-right corner
    static bool firstShown = true;
    if (firstShown && m_ShowShaderEditor)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 workPos = viewport->WorkPos;
        ImVec2 workSize = viewport->WorkSize;
        float windowWidth = 600.0f;
        float windowHeight = workSize.y * 0.5f; // 50% of window height
        
        ImGui::SetNextWindowPos(
            ImVec2(workPos.x + workSize.x - windowWidth - panelMargin, workPos.y + panelMargin), 
            ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);
        firstShown = false;
    }
    
    ImGui::Begin("Shader Editor", &m_ShowShaderEditor);
    
    // Show a notice if no object is selected
    if (!m_SelectedObject)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), 
            "No object selected. Please select an object in the Object Properties panel.");
        ImGui::Separator();
    }
    
    // Show compilation status messages
    if (!m_CompilationMessage.empty())
    {
        if (m_CompilationSuccessful)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", m_CompilationMessage.c_str());
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", m_CompilationMessage.c_str());
        }
        ImGui::Separator();
    }
    
    static bool firstTime = true;
    if (firstTime)
    {
        // Load default shaders
        std::ifstream vertFile("resources/shaders/default.vert");
        if (vertFile.is_open())
        {
            std::stringstream buffer;
            buffer << vertFile.rdbuf();
            m_VertexShaderSource = buffer.str();
            vertFile.close();
        }
        
        std::ifstream fragFile("resources/shaders/default.frag");
        if (fragFile.is_open())
        {
            std::stringstream buffer;
            buffer << fragFile.rdbuf();
            m_FragmentShaderSource = buffer.str();
            fragFile.close();
        }
        
        firstTime = false;
    }
    
    // Set up some reasonable sizes for the editor
    ImVec2 size = ImGui::GetContentRegionAvail();
    size.y = size.y / 2.0f - 45.0f;
    
    ImGui::Text("Vertex Shader");
    // Use InputTextMultiline with a callback that properly handles std::string
    if (ImGui::InputTextMultiline("##VertexShader", 
                                 const_cast<char*>(m_VertexShaderSource.data()), 
                                 m_VertexShaderSource.capacity() + 1, 
                                 size, 
                                 ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize,
                                 [](ImGuiInputTextCallbackData* data) -> int {
                                     if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                                     {
                                         std::string* str = static_cast<std::string*>(data->UserData);
                                         str->resize(data->BufTextLen);
                                         data->Buf = const_cast<char*>(str->c_str());
                                     }
                                     return 0;
                                 }, &m_VertexShaderSource))
    {
        m_ShaderModified = true;
    }
    
    ImGui::Text("Fragment Shader");
    // Use InputTextMultiline with a callback that properly handles std::string
    if (ImGui::InputTextMultiline("##FragmentShader", 
                                 const_cast<char*>(m_FragmentShaderSource.data()), 
                                 m_FragmentShaderSource.capacity() + 1, 
                                 size, 
                                 ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize,
                                 [](ImGuiInputTextCallbackData* data) -> int {
                                     if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                                     {
                                         std::string* str = static_cast<std::string*>(data->UserData);
                                         str->resize(data->BufTextLen);
                                         data->Buf = const_cast<char*>(str->c_str());
                                     }
                                     return 0;
                                 }, &m_FragmentShaderSource))
    {
        m_ShaderModified = true;
    }
    
    // Show status indicator if the shader has been modified
    if (m_ShaderModified)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Shader modified - Click 'Compile Shader' to apply changes");
    }
    
    // Compile button (disabled if no object is selected)
    if (!m_SelectedObject)
    {
        ImGui::BeginDisabled();
    }
    
    if (ImGui::Button("Compile Shader") || ImGui::IsKeyPressed(ImGuiKey_F5))
    {
        if (m_OnCompileShader && m_SelectedObject)
        {
            Shader* shader = m_SelectedObject->GetShader();
            if (shader)
            {
                // Try to compile the shader
                bool success = m_OnCompileShader(shader, m_VertexShaderSource, m_FragmentShaderSource);
                if (success)
                {
                    m_ShaderModified = false;
                    m_CompilationSuccessful = true;
                    m_CompilationMessage = "Shader compiled successfully!";
                }
                else
                {
                    m_CompilationSuccessful = false;
                    m_CompilationMessage = "Failed to compile shader: " + shader->GetCompilationLog();
                }
            }
            else
            {
                m_CompilationSuccessful = false;
                m_CompilationMessage = "Object does not have a shader attached!";
            }
        }
    }
    
    if (!m_SelectedObject)
    {
        ImGui::EndDisabled();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Reset to Default"))
    {
        // Load default shaders again
        std::ifstream vertFile("resources/shaders/default.vert");
        if (vertFile.is_open())
        {
            std::stringstream buffer;
            buffer << vertFile.rdbuf();
            m_VertexShaderSource = buffer.str();
            vertFile.close();
        }
        
        std::ifstream fragFile("resources/shaders/default.frag");
        if (fragFile.is_open())
        {
            std::stringstream buffer;
            buffer << fragFile.rdbuf();
            m_FragmentShaderSource = buffer.str();
            fragFile.close();
        }
        
        m_ShaderModified = true;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Clear Message"))
    {
        m_CompilationMessage.clear();
    }
    
    ImGui::End();
}

void UI::RenderObjectProperties()
{
    // Position the Object Properties panel below the Shader Editor, taking up left half
    static bool firstShown = true;
    if (firstShown && m_ShowObjectProperties)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 workPos = viewport->WorkPos;
        ImVec2 workSize = viewport->WorkSize;
        
        // Calculate position based on Shader Editor position (top-right)
        float shaderEditorWidth = 600.0f;
        float shaderEditorHeight = workSize.y * 0.5f;
        float windowWidth = shaderEditorWidth / 2.0f; // Half of shader editor width
        float windowHeight = workSize.y * 0.45f; // Fill remaining height with some padding
        
        // Position below shader editor, on the left half
        ImGui::SetNextWindowPos(
            ImVec2(workPos.x + workSize.x - shaderEditorWidth - panelMargin, workPos.y + shaderEditorHeight + panelMargin), 
            ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);
        firstShown = false;
    }
    
    ImGui::Begin("Object Properties", &m_ShowObjectProperties);
    
    Application* app = Application::GetInstance();
    if (!app || !app->GetScene())
    {
        ImGui::Text("No scene available");
        ImGui::End();
        return;
    }
    
    Scene* scene = app->GetScene();
    
    // Object selection
    if (ImGui::BeginCombo("Select Object", m_SelectedObject ? m_SelectedObject->GetName().c_str() : "None"))
    {
        for (const auto& object : scene->GetObjects())
        {
            bool isSelected = (m_SelectedObject == object.get());
            if (ImGui::Selectable(object->GetName().c_str(), isSelected))
            {
                m_SelectedObject = object.get();
            }
            
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    
    // Object properties editing
    if (m_SelectedObject)
    {
        ImGui::Separator();
        
        // Edit name
        std::string objectName = m_SelectedObject->GetName();
        static char nameBuffer[256] = "";
        if (objectName.size() < sizeof(nameBuffer)) {
            strcpy(nameBuffer, objectName.c_str());
        }
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
        {
            m_SelectedObject->SetName(nameBuffer);
        }
        
        // Visibility
        bool visible = m_SelectedObject->IsVisible();
        if (ImGui::Checkbox("Visible", &visible))
        {
            m_SelectedObject->SetVisible(visible);
        }
        
        ImGui::Separator();
        
        // Transform properties
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Position
            glm::vec3 position = m_SelectedObject->GetPosition();
            if (ImGui::DragFloat3("Position", &position[0], 0.01f))
            {
                m_SelectedObject->SetPosition(position);
            }
            
            // Rotation
            glm::vec3 rotation = m_SelectedObject->GetRotation();
            if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f))
            {
                m_SelectedObject->SetRotation(rotation);
            }
            
            // Scale
            glm::vec3 scale = m_SelectedObject->GetScale();
            if (ImGui::DragFloat3("Scale", &scale[0], 0.01f, 0.01f, 100.0f))
            {
                m_SelectedObject->SetScale(scale);
            }
        }
        
        // Material properties
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Material& material = m_SelectedObject->GetMaterial();
            
            ImGui::ColorEdit3("Ambient", &material.ambient[0]);
            ImGui::ColorEdit3("Diffuse", &material.diffuse[0]);
            ImGui::ColorEdit3("Specular", &material.specular[0]);
            ImGui::SliderFloat("Shininess", &material.shininess, 1.0f, 256.0f);
        }
    }
    else
    {
        ImGui::Text("No object selected");
    }
    
    ImGui::End();
}

void UI::RenderSceneSettings()
{
    // Position the Scene Settings panel below the Shader Editor, taking up right half
    static bool firstShown = true;
    if (firstShown && m_ShowSceneSettings)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 workPos = viewport->WorkPos;
        ImVec2 workSize = viewport->WorkSize;
        
        // Calculate position based on Shader Editor position (top-right)
        float shaderEditorWidth = 600.0f;
        float shaderEditorHeight = workSize.y * 0.5f;
        float windowWidth = shaderEditorWidth / 2.0f; // Half of shader editor width
        float windowHeight = workSize.y * 0.45f; // Fill remaining height with some padding
        
        // Position below shader editor, on the right half
        ImGui::SetNextWindowPos(
            ImVec2(workPos.x + workSize.x - windowWidth - panelMargin, workPos.y + shaderEditorHeight + panelMargin), 
            ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);
        firstShown = false;
    }
    
    ImGui::Begin("Scene Settings", &m_ShowSceneSettings);
    
    Application* app = Application::GetInstance();
    if (!app || !app->GetScene() || !app->GetRenderer())
    {
        ImGui::Text("No scene or renderer available");
        ImGui::End();
        return;
    }
    
    Scene* scene = app->GetScene();
    Renderer* renderer = app->GetRenderer();
    
    // Clear color
    glm::vec4 clearColor = renderer->GetClearColor();
    if (ImGui::ColorEdit3("Background Color", &clearColor[0]))
    {
        renderer->SetClearColor(clearColor);
    }
    
    ImGui::Separator();
    
    // Light settings
    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const std::vector<Light>& lights = scene->GetLights();
        
        for (int i = 0; i < lights.size(); i++)
        {
            ImGui::PushID(i);
            
            std::string lightLabel = "Light " + std::to_string(i + 1);
            if (ImGui::TreeNode(lightLabel.c_str()))
            {
                Light* light = scene->GetLight(i);
                if (light)
                {
                    ImGui::DragFloat3("Position", &light->position[0], 0.1f);
                    ImGui::ColorEdit3("Color", &light->color[0]);
                    ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 10.0f);
                }
                
                ImGui::TreePop();
            }
            
            ImGui::PopID();
        }
        
        if (ImGui::Button("Add Light"))
        {
            Light newLight;
            scene->AddLight(newLight);
        }
    }
    
    ImGui::End();
}

void UI::RenderPerformanceOverlay()
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings |
                                  ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav;
    
    if (corner != -1)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - DISTANCE) : (work_pos.x + DISTANCE);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - DISTANCE) : (work_pos.y + DISTANCE);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Performance Overlay", &m_ShowPerformanceOverlay, window_flags))
    {
        ImGui::Text("Performance");
        ImGui::Separator();
        ImGui::Text("FPS: %.1f", 1.0f / m_AverageFrameTime);
        ImGui::Text("Frame Time: %.3f ms", m_AverageFrameTime * 1000.0f);
        
        ImGui::Separator();
        
        ImGui::Text("Frame Time Graph");
        char overlay[32];
        sprintf(overlay, "%.3f ms", m_AverageFrameTime * 1000.0f);
        ImGui::PlotLines("##FrameTimes", m_FrameTimes, IM_ARRAYSIZE(m_FrameTimes), m_FrameTimeIndex, 
                       overlay, 0.0f, 0.040f, ImVec2(0, 80));
        
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (ImGui::MenuItem("Close")) m_ShowPerformanceOverlay = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

// Open file dialog for loading a scene
std::string UI::OpenFileDialog(const char* filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(m_Window);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    
    // Get the current working directory
    char currentDir[MAX_PATH];
    _getcwd(currentDir, MAX_PATH);

    // Set initial directory to the resources/scenes folder
    std::string initialDir = std::string(currentDir) + "\\resources\\scenes";
    ofn.lpstrInitialDir = initialDir.c_str();
    
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    
    return "";
}

// Save file dialog for saving a scene
std::string UI::SaveFileDialog(const char* filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(m_Window);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    
    // Get the current working directory
    char currentDir[MAX_PATH];
    _getcwd(currentDir, MAX_PATH);

    // Set initial directory to the resources/scenes folder
    std::string initialDir = std::string(currentDir) + "\\resources\\scenes";
    ofn.lpstrInitialDir = initialDir.c_str();
    
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileNameA(&ofn) == TRUE)
    {
        // Check if the filename has the .json extension
        std::string filename = ofn.lpstrFile;
        if (filename.substr(filename.find_last_of(".") + 1) != "json")
        {
            // Add .json extension if needed
            filename += ".json";
        }
        
        return filename;
    }
    
    return "";
}

// Method for opening a model file dialog
std::string UI::OpenModelDialog()
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(m_Window);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "3D Model Files\0*.obj;*.fbx;*.gltf;*.glb;*.3ds;*.dae\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    
    // Get the current working directory
    char currentDir[MAX_PATH];
    _getcwd(currentDir, MAX_PATH);

    // Set initial directory to the resources/models folder
    // Create the directory if it doesn't exist
    std::string modelsDir = std::string(currentDir) + "\\resources\\models";
    std::filesystem::create_directories(modelsDir);
    
    ofn.lpstrInitialDir = modelsDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    
    return "";
}

// Helper method to import a 3D model
void UI::ImportCustomModel(const std::string& filepath)
{
    if (filepath.empty() || !m_OnImportModel)
        return;
        
    // Call the import model callback with the filepath
    m_OnImportModel(filepath);
}

bool UI::IsCapturingKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool UI::IsCapturingMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}