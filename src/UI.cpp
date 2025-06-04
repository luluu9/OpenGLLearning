#include "UI.h"
#include "Application.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Shader.h"
#include "Primitives.h"
#include "ResourceManager.h"

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

bool UI::Initialize(GLFWwindow* newWindow)
{
    if (!newWindow)
        return false;
        
    window = newWindow;
    
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
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    
    // Store frame time for performance monitoring
    frameTimes[frameTimeIndex] = deltaTime;
    frameTimeIndex = (frameTimeIndex + 1) % IM_ARRAYSIZE(frameTimes);
    
    // Calculate average frame time
    frameTimeAccumulator += deltaTime;
    frameCount++;
    
    if (frameTimeAccumulator >= 0.5f) // Update average every half second
    {
        averageFrameTime = frameTimeAccumulator / frameCount;
        frameTimeAccumulator = 0.0f;
        frameCount = 0;
    }

    // Render ImGui components
    RenderMainMenuBar();
    
    if (showShaderEditor)
        RenderShaderEditor();
        
    if (showObjectProperties)
        RenderObjectProperties();
        
    if (showSceneSettings)
        RenderSceneSettings();
        
    if (showPerformanceOverlay)
        RenderPerformanceOverlay();
        
    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);
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
                    
                    selectedObject = nullptr;
                }
            }
            
            if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
            {
                // Show file dialog to open scene
                std::string filepath = OpenFileDialog("JSON Scene Files\0*.json\0All Files\0*.*\0");
                if (!filepath.empty() && onSceneLoad)
                {
                    onSceneLoad(filepath);
                    selectedObject = nullptr; // Reset selected object
                }
            }
            
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            {
                // Show file dialog to save scene
                std::string filepath = SaveFileDialog("JSON Scene Files\0*.json\0All Files\0*.*\0");
                if (!filepath.empty() && onSceneSave)
                {
                    onSceneSave(filepath);
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Shader Editor", nullptr, showShaderEditor))
                showShaderEditor = !showShaderEditor;
                
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Object Properties", nullptr, showObjectProperties))
                showObjectProperties = !showObjectProperties;
                
            if (ImGui::MenuItem("Scene Settings", nullptr, showSceneSettings))
                showSceneSettings = !showSceneSettings;
                
            if (ImGui::MenuItem("Performance Overlay", nullptr, showPerformanceOverlay))
                showPerformanceOverlay = !showPerformanceOverlay;
                
            ImGui::Separator();
            
            if (ImGui::MenuItem("ImGui Demo", nullptr, showDemoWindow))
                showDemoWindow = !showDemoWindow;
                
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Objects"))
        {
            if (ImGui::MenuItem("Add Cube"))
            {
                if (onAddObject)
                    onAddObject("Cube");
            }
            
            if (ImGui::MenuItem("Add Sphere"))
            {
                if (onAddObject)
                    onAddObject("Sphere");
            }
            
            if (ImGui::MenuItem("Add Plane"))
            {
                if (onAddObject)
                    onAddObject("Plane");
            }
            
            if (ImGui::MenuItem("Add Cylinder"))
            {
                if (onAddObject)
                    onAddObject("Cylinder");
            }
            
            if (ImGui::MenuItem("Add Cone"))
            {
                if (onAddObject)
                    onAddObject("Cone");
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
                        
                    bool isDeferred = renderer->GetRenderMode() == RenderMode::Deferred;
                    if (ImGui::MenuItem("Deferred", nullptr, isDeferred))
                        renderer->SetRenderMode(RenderMode::Deferred);
                        
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Lighting Model"))
                {
                    bool isFlat = renderer->GetLightingModel() == LightingModel::Flat;
                    if (ImGui::MenuItem("Flat", nullptr, isFlat))
                        renderer->SetLightingModel(LightingModel::Flat);
                        
                    bool isPhong = renderer->GetLightingModel() == LightingModel::Phong;
                    if (ImGui::MenuItem("Blinn-Phong", nullptr, isPhong))
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
    if (firstShown && showShaderEditor)
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
    
    ImGui::Begin("Shader Editor", &showShaderEditor);
    
    // Show a notice if no object is selected
    if (!selectedObject)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), 
            "No object selected. Please select an object in the Object Properties panel.");
        ImGui::Separator();
    }
    
    // Show compilation status messages
    if (!compilationMessage.empty())
    {
        if (compilationSuccessful)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", compilationMessage.c_str());
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", compilationMessage.c_str());
        }
        ImGui::Separator();
    }
    
    // Tab bar for switching between editor and shader library
    if (ImGui::BeginTabBar("ShaderEditorTabs"))
    {
        // Custom Shader Editor Tab
        if (ImGui::BeginTabItem("Shader Editor"))
        {
            static bool firstTime = true;
            if (firstTime)
            {
                // Load default shaders
                std::ifstream vertFile("resources/shaders/default.vert");
                if (vertFile.is_open())
                {
                    std::stringstream buffer;
                    buffer << vertFile.rdbuf();
                    vertexShaderSource = buffer.str();
                    vertFile.close();
                }
                
                std::ifstream fragFile("resources/shaders/default.frag");
                if (fragFile.is_open())
                {
                    std::stringstream buffer;
                    buffer << fragFile.rdbuf();
                    fragmentShaderSource = buffer.str();
                    fragFile.close();
                }
                
                firstTime = false;
            }
            
            // Switch to the editor view
            useShaderLibrary = false;
            
            ImGui::EndTabItem();
        }
        
        // Shader Library Tab
        if (ImGui::BeginTabItem("Shader Library"))
        {
            // Ensure we have the latest shaders
            if (allShaders.empty())
            {
                RefreshShaderLibrary();
            }
            
            // Switch to the library view
            useShaderLibrary = true;
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
      // Only show the editor content if we're not in library mode
    if (!useShaderLibrary)
    {
        // Set up some reasonable sizes for the editor
        ImVec2 size = ImGui::GetContentRegionAvail();
        size.y = size.y / 2.0f - 45.0f;
    
        ImGui::Text("Vertex Shader");
        // Use InputTextMultiline with a callback that properly handles std::string
        if (ImGui::InputTextMultiline("##VertexShader", 
                                    const_cast<char*>(vertexShaderSource.data()), 
                                    vertexShaderSource.capacity() + 1, 
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
                                    }, &vertexShaderSource))
        {
            shaderModified = true;
        }
        
        ImGui::Text("Fragment Shader");
        // Use InputTextMultiline with a callback that properly handles std::string
        if (ImGui::InputTextMultiline("##FragmentShader", 
                                    const_cast<char*>(fragmentShaderSource.data()), 
                                    fragmentShaderSource.capacity() + 1, 
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
                                    }, &fragmentShaderSource))
        {
            shaderModified = true;
        }
        
        // Show status indicator if the shader has been modified
        if (shaderModified)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Shader modified - Click 'Compile Shader' to apply changes");
        }
        
        // Compile button (disabled if no object is selected)
        if (!selectedObject)
        {
            ImGui::BeginDisabled();
        }
        
        if (ImGui::Button("Compile Shader") || ImGui::IsKeyPressed(ImGuiKey_F5))
        {
            if (onCompileShader && selectedObject)
            {
                Shader* shader = selectedObject->GetShader();
                if (shader)
                {
                    // Try to compile the shader
                    bool success = onCompileShader(shader, vertexShaderSource, fragmentShaderSource);
                    if (success)
                    {
                        shaderModified = false;
                        compilationSuccessful = true;
                        compilationMessage = "Shader compiled successfully!";
                    }
                    else
                    {
                        compilationSuccessful = false;
                        compilationMessage = "Failed to compile shader: " + shader->GetCompilationLog();
                    }
                }
                else
                {
                    compilationSuccessful = false;
                    compilationMessage = "Object does not have a shader attached!";
                }
            }
        }
        if (!selectedObject)
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
                vertexShaderSource = buffer.str();
                vertFile.close();
            }
            
            std::ifstream fragFile("resources/shaders/default.frag");
            if (fragFile.is_open())
            {
                std::stringstream buffer;
                buffer << fragFile.rdbuf();
                fragmentShaderSource = buffer.str();
                fragFile.close();
            }
            
            shaderModified = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Message"))
        {
            compilationMessage.clear();
        }
    } 
    // Render the shader library UI only if enabled
    else
    {
        if (ImGui::Button("Load All Shaders"))
        {
            ResourceManager* resourceManager = ResourceManager::GetInstance();
            if (resourceManager->LoadAllShadersFromDirectory("resources/shaders"))
            {
                RefreshShaderLibrary();
                compilationMessage = "All shaders loaded successfully";
                compilationSuccessful = true;
            }
            else
            {
                compilationMessage = "Some shaders failed to load. Check console for details.";
                compilationSuccessful = false;
            }
        }
        
        ImGui::Separator();
        ImGui::Text("Categories:");
        
        // Display categories as a list of selectable items
        float categoryWidth = ImGui::GetContentRegionAvail().x * 0.25f;
        ImGui::BeginChild("CategoryList", ImVec2(categoryWidth, ImGui::GetContentRegionAvail().y * 0.8f), true);
        
        // Always show "All" category first
        if (ImGui::Selectable("All", currentShaderCategory == "All"))
        {
            currentShaderCategory = "All";
        }
        
        // Show other categories
        for (const auto& [category, shaders] : shaderCategories)
        {
            if (category != "All")
            {
                if (ImGui::Selectable(category.c_str(), currentShaderCategory == category))
                {
                    currentShaderCategory = category;
                }
            }
        }
        
        ImGui::EndChild();
        
        // Shader list for the selected category
        ImGui::SameLine();
        ImGui::BeginChild("ShaderList", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.8f), true);
        
        const auto& shadersInCategory = shaderCategories[currentShaderCategory];
        if (shadersInCategory.empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "No shaders in this category.");
        }
        else
        {
            ResourceManager* resourceManager = ResourceManager::GetInstance();
            
            for (const auto& shaderName : shadersInCategory)
            {
                ImGui::PushID(shaderName.c_str());
                
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", shaderName.c_str());
                
                if (!selectedObject)
                {
                    ImGui::BeginDisabled();
                }
                
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - 100.0f);
                if (ImGui::Button("Apply", ImVec2(100.0f, 0)))
                {
                    if (selectedObject && onSelectShader)
                    {
                        onSelectShader(shaderName);
                        compilationMessage = "Applied shader: " + shaderName;
                        compilationSuccessful = true;
                        
                        // Load the shader source into the editor
                        ResourceManager* resourceManager = ResourceManager::GetInstance();
                        const auto& shaderInfo = resourceManager->GetShaderInfo(shaderName);
                        
                        if (!shaderInfo.vertexPath.empty() && !shaderInfo.fragmentPath.empty())
                        {
                            // Load vertex shader source
                            std::ifstream vertFile(shaderInfo.vertexPath);
                            if (vertFile.is_open())
                            {
                                std::stringstream buffer;
                                buffer << vertFile.rdbuf();
                                vertexShaderSource = buffer.str();
                                vertFile.close();
                            }
                            
                            // Load fragment shader source
                            std::ifstream fragFile(shaderInfo.fragmentPath);
                            if (fragFile.is_open())
                            {
                                std::stringstream buffer;
                                buffer << fragFile.rdbuf();
                                fragmentShaderSource = buffer.str();
                                fragFile.close();
                            }
                            
                            // Switch to editor mode
                            useShaderLibrary = false;
                        }
                    }
                }
                  if (!selectedObject)
                {
                    ImGui::EndDisabled();
                }
                
                if (ImGui::IsItemHovered())
                {
                    // Get the actual description from ShaderInfo instead of the category
                    auto it = resourceManager->GetShaderInfo(shaderName);
                    if (!it.description.empty())
                    {
                        ImGui::SetTooltip("%s", it.description.c_str());
                    }
                    else
                    {
                        ImGui::SetTooltip("Category: %s", resourceManager->GetShaderCategory(shaderName).c_str());
                    }
                }
                
                ImGui::PopID();
            }
        }
        
        ImGui::EndChild();
    }
    
    ImGui::End();
}

void UI::RenderObjectProperties()
{
    // Position the Object Properties panel below the Shader Editor, taking up left half
    static bool firstShown = true;
    if (firstShown && showObjectProperties)
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
    
    ImGui::Begin("Object Properties", &showObjectProperties);
    
    Application* app = Application::GetInstance();
    if (!app || !app->GetScene())
    {
        ImGui::Text("No scene available");
        ImGui::End();
        return;
    }
    
    Scene* scene = app->GetScene();
    // Object selection
    if (ImGui::BeginCombo("Select Object", selectedObject ? selectedObject->GetName().c_str() : "None"))
    {  
        for (const auto& object : scene->GetObjects())
        {
            bool isSelected = (selectedObject == object.get());
            if (ImGui::Selectable(object->GetName().c_str(), isSelected))
            {
                // Unhighlight previous selection
                if (selectedObject && selectedObject->IsHighlighted())
                {
                    selectedObject->SetHighlighted(false);
                }
                
                selectedObject = object.get();
                
                // Highlight new selection
                if (selectedObject)
                {
                    selectedObject->SetHighlighted(true);
                }
            }
            
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    
    // Object properties editing
    if (selectedObject)
    {
        ImGui::Separator();
        
        // Edit name
        std::string objectName = selectedObject->GetName();
        static char nameBuffer[256] = "";
        if (objectName.size() < sizeof(nameBuffer)) {
            strcpy(nameBuffer, objectName.c_str());
        }
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
        {
            selectedObject->SetName(nameBuffer);
        }
        
        // Visibility
        bool visible = selectedObject->IsVisible();
        if (ImGui::Checkbox("Visible", &visible))
        {
            selectedObject->SetVisible(visible);
        }
        
        ImGui::Separator();
        
        // Transform properties
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Position
            glm::vec3 position = selectedObject->GetPosition();
            if (ImGui::DragFloat3("Position", &position[0], 0.01f))
            {
                selectedObject->SetPosition(position);
            }
            
            // Rotation
            glm::vec3 rotation = selectedObject->GetRotation();
            if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f))
            {
                selectedObject->SetRotation(rotation);
            }
            
            // Scale
            glm::vec3 scale = selectedObject->GetScale();
            if (ImGui::DragFloat3("Scale", &scale[0], 0.01f, 0.01f, 100.0f))
            {
                selectedObject->SetScale(scale);
            }
        }
        
        // Material properties
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Material& material = selectedObject->GetMaterial();
            
            ImGui::ColorEdit3("Ambient", &material.ambient[0]);
            ImGui::ColorEdit3("Diffuse", &material.diffuse[0]);
            ImGui::ColorEdit3("Specular", &material.specular[0]);
            ImGui::SliderFloat("Shininess", &material.shininess, 1.0f, 256.0f);
        }

        if (ImGui::Button("Unselect"))
        {
            // Unhighlight current selection
            if (selectedObject && selectedObject->IsHighlighted())
            {
                selectedObject->SetHighlighted(false);
            }
            selectedObject = nullptr;
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
    if (firstShown && showSceneSettings)
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
    
    ImGui::Begin("Scene Settings", &showSceneSettings);
    
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
    if (ImGui::Begin("Performance Overlay", &showPerformanceOverlay, window_flags))
    {
        ImGui::Text("Performance");
        ImGui::Separator();
        ImGui::Text("FPS: %.1f", 1.0f / averageFrameTime);
        
        ImGui::Text("Frame Time Graph");
        char overlay[32];
        sprintf(overlay, "%.3f ms", averageFrameTime * 1000.0f);
        ImGui::PlotLines("##FrameTimes", frameTimes, IM_ARRAYSIZE(frameTimes), frameTimeIndex, 
                       overlay, 0.0f, 0.040f, ImVec2(0, 80));
        
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (ImGui::MenuItem("Close")) showPerformanceOverlay = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void UI::RefreshShaderLibrary()
{
    // Get all shader names from ResourceManager
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    allShaders = resourceManager->GetAllShaderNames();
    
    // Clear existing categories
    shaderCategories.clear();
    shaderCategories["All"] = allShaders;
    
    // Group shaders by category
    for (const auto& shaderName : allShaders)
    {
        std::string category = resourceManager->GetShaderCategory(shaderName);
        shaderCategories[category].push_back(shaderName);
    }
    
    // If no category is selected or the selected category no longer exists, default to "All"
    if (shaderCategories.find(currentShaderCategory) == shaderCategories.end())
    {
        currentShaderCategory = "All";
    }
}

std::string UI::OpenFileDialog(const char* filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(window);
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

std::string UI::SaveFileDialog(const char* filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(window);
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

std::string UI::OpenModelDialog()
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = glfwGetWin32Window(window);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "3D Model Files\0*.obj;*.fbx;*.gltf;*.glb;*.3ds;*.dae\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    
    // Get the current working directory
    char currentDir[MAX_PATH];
    _getcwd(currentDir, MAX_PATH);

    std::string modelsDir = std::string(currentDir) + "\\resources\\models";
    ofn.lpstrInitialDir = modelsDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    
    return "";
}

void UI::ImportCustomModel(const std::string& filepath)
{
    if (filepath.empty() || !onImportModel)
        return;
        
    onImportModel(filepath);
}

bool UI::IsCapturingKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool UI::IsCapturingMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}