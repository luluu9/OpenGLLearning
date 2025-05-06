#include "Application.h"
#include "Primitives.h"
#include "ResourceManager.h"
#include "SceneObject.h"
#include "Shader.h"
#include <iostream>
#include <memory>
#include <string>

// Application-wide callbacks
bool CompileAndSetShader(SceneObject* object, const std::string& vertexSource, const std::string& fragmentSource);

int main()
{
    // Create application instance
    Application app("OpenGL Learning", 1920, 1080);
    
    // Initialize application
    if (!app.Initialize())
    {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }
    
    // Set up UI callbacks
    app.GetUI()->SetOnAddObjectCallback([](const std::string& type) {
        Application* app = Application::GetInstance();
        if (!app || !app->GetScene())
            return;
            
        std::string name = type + "_" + std::to_string(app->GetScene()->GetObjects().size() + 1);
        auto object = Primitives::CreatePrimitiveObject(type, name);
        
        if (object)
            app->GetScene()->AddObject(std::move(object));
    });
    
    app.GetUI()->SetOnCompileShaderCallback([](Shader* shader, const std::string& vertexSource, const std::string& fragmentSource) {
        if (!shader)
            return false;
            
        return shader->LoadFromSource(vertexSource, fragmentSource);
    });
    
    app.GetUI()->SetOnSceneLoadCallback([](const std::string& filepath) {
        Application* app = Application::GetInstance();
        if (!app || !app->GetScene())
            return;
            
        app->GetScene()->LoadFromFile(filepath);
    });
    
    app.GetUI()->SetOnSceneSaveCallback([](const std::string& filepath) {
        Application* app = Application::GetInstance();
        if (!app || !app->GetScene())
            return;
            
        app->GetScene()->SaveToFile(filepath);
    });
    
    // Add default objects to the scene
    auto cube = Primitives::CreatePrimitiveObject("Cube", "Cube_1");
    cube->SetPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
    app.GetScene()->AddObject(std::move(cube));
    
    auto sphere = Primitives::CreatePrimitiveObject("Sphere", "Sphere_1");
    sphere->SetPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    app.GetScene()->AddObject(std::move(sphere));
    
    // Run the application
    app.Run();
    
    return 0;
}

bool CompileAndSetShader(SceneObject* object, const std::string& vertexSource, const std::string& fragmentSource)
{
    if (!object)
        return false;
        
    // Generate a unique name for the shader based on the object name
    std::string shaderName = object->GetName() + "_shader";
    
    // Get the resource manager
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    
    // Load the shader from source
    Shader* shader = resourceManager->LoadShaderFromSource(shaderName, vertexSource, fragmentSource);
    
    if (!shader)
    {
        std::cerr << "Failed to compile shader for " << object->GetName() << std::endl;
        return false;
    }
    
    // Set the shader to the object
    object->SetShader(shader);
    
    return true;
}