#include "Application.h"
#include "Primitives.h"
#include "SceneObject.h"
#include "Shader.h"
#include <iostream>
#include <memory>
#include <string>

// Application-wide callbacks
std::unique_ptr<SceneObject> CreatePrimitiveObject(const std::string& type, const std::string& name);
bool CompileAndSetShader(SceneObject* object, const std::string& vertexSource, const std::string& fragmentSource);

int main()
{
    // Create application instance
    Application app("OpenGL Learning", 1280, 720);
    
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
        auto object = CreatePrimitiveObject(type, name);
        
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
    auto cube = CreatePrimitiveObject("Cube", "Cube_1");
    cube->SetPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
    app.GetScene()->AddObject(std::move(cube));
    
    auto sphere = CreatePrimitiveObject("Sphere", "Sphere_1");
    sphere->SetPosition(glm::vec3(1.5f, 0.0f, 0.0f));
    app.GetScene()->AddObject(std::move(sphere));
    
    // Run the application
    app.Run();
    
    return 0;
}

std::unique_ptr<SceneObject> CreatePrimitiveObject(const std::string& type, const std::string& name)
{
    // Create a new scene object with the specified name
    auto object = std::make_unique<SceneObject>(name);
    
    // Create a mesh based on the primitive type
    std::unique_ptr<Mesh> mesh;
    
    if (type == "Cube")
    {
        mesh = Primitives::CreateCube();
    }
    else if (type == "Sphere")
    {
        mesh = Primitives::CreateSphere();
    }
    else if (type == "Plane")
    {
        mesh = Primitives::CreatePlane();
    }
    else if (type == "Cylinder")
    {
        mesh = Primitives::CreateCylinder();
    }
    else if (type == "Cone")
    {
        mesh = Primitives::CreateCone();
    }
    else
    {
        std::cerr << "Unknown primitive type: " << type << std::endl;
        return nullptr;
    }
    
    // Set the mesh to the object
    object->SetMesh(std::move(mesh));
    
    // Create and compile default shader
    auto shader = std::make_unique<Shader>();
    bool success = shader->LoadFromFile("resources/shaders/default.vert", "resources/shaders/default.frag");
    
    if (!success)
    {
        std::cerr << "Failed to load default shaders for " << name << std::endl;
        std::cerr << shader->GetCompilationLog() << std::endl;
        return nullptr;
    }
    
    // Set the shader to the application's shared shader pool (not implemented in this example)
    // In a real application, you might want to manage shaders in a resource manager to avoid duplication
    // For simplicity, we'll attach the shader directly to the object
    Application* app = Application::GetInstance();
    if (app)
    {
        static std::vector<std::unique_ptr<Shader>> shaderPool;
        shaderPool.push_back(std::move(shader));
        object->SetShader(shaderPool.back().get());
    }
    
    // Set some default material properties
    Material material;
    material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    material.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 32.0f;
    
    object->SetMaterial(material);
    
    return object;
}

bool CompileAndSetShader(SceneObject* object, const std::string& vertexSource, const std::string& fragmentSource)
{
    if (!object)
        return false;
        
    Shader* shader = object->GetShader();
    if (!shader)
    {
        // Create a new shader if the object doesn't have one
        auto newShader = std::make_unique<Shader>();
        bool success = newShader->LoadFromSource(vertexSource, fragmentSource);
        
        if (!success)
        {
            std::cerr << "Failed to compile shader: " << newShader->GetCompilationLog() << std::endl;
            return false;
        }
        
        // Add to application's shader pool (not implemented in this example)
        static std::vector<std::unique_ptr<Shader>> shaderPool;
        shaderPool.push_back(std::move(newShader));
        object->SetShader(shaderPool.back().get());
    }
    else
    {
        // Use the existing shader
        bool success = shader->LoadFromSource(vertexSource, fragmentSource);
        
        if (!success)
        {
            std::cerr << "Failed to compile shader: " << shader->GetCompilationLog() << std::endl;
            return false;
        }
    }
    
    return true;
}