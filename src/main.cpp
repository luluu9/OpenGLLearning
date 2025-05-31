#include "Application.h"
#include "Primitives.h"
#include "ResourceManager.h"
#include "SceneObject.h"
#include "Shader.h"
#include "Model.h"
#include <iostream>
#include <memory>
#include <string>
#include <filesystem>


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
    
    // Add model import callback
    app.GetUI()->SetOnImportModelCallback([](const std::string& filepath) {
        Application* app = Application::GetInstance();
        if (!app || !app->GetScene())
            return;
            
        std::filesystem::path path(filepath);
        std::string filename = path.filename().string();
        std::string modelName = filename.substr(0, filename.find_last_of('.'));
        std::string objectName = "Model_" + modelName + "_" + std::to_string(app->GetScene()->GetObjects().size() + 1);
        
        auto object = std::make_unique<SceneObject>(objectName);
        
        // Convert backslashes to forward slashes for cross-platform compatibility
        std::string normalizedPath = filepath;
        std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');
        
        ResourceManager* resourceManager = ResourceManager::GetInstance();
        Model* model = resourceManager->LoadModel(modelName, normalizedPath);
        
        if (model)
        {
            object->SetModel(model);
            
            Shader* shader = resourceManager->GetShader("default");
            if (!shader)
            {
                shader = resourceManager->LoadShaderFromFile("default", 
                    "resources/shaders/default.vert", 
                    "resources/shaders/default.frag");
            }
            if (shader)
                object->SetShader(shader);
                
            Primitives::SetupDefaultMaterial(object.get());
            app->GetScene()->AddObject(std::move(object));
            
            std::cout << "Successfully imported model: " << normalizedPath << std::endl;
        }
        else
        {
            std::cerr << "Failed to import model: " << normalizedPath << std::endl;
        }
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