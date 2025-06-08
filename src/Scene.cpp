#include "Scene.h"
#include "SceneObject.h"
#include "Primitives.h"
#include "ResourceManager.h"
#include "Shader.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

Scene::Scene() { 
    AddDefaultLight();
}

Scene::~Scene() = default;

void Scene::Update(float deltaTime)
{
    for (auto& object : objects)
    {
        if (object)
            object->Update(deltaTime);
    }
}

SceneObject* Scene::AddObject(std::unique_ptr<SceneObject> object)
{
    if (!object)
        return nullptr;
        
    objects.push_back(std::move(object));
    return objects.back().get();
}

void Scene::RemoveObject(SceneObject* object)
{
    if (!object)
        return;
        
    auto it = std::find_if(objects.begin(), objects.end(),
                         [object](const std::unique_ptr<SceneObject>& obj) {
                             return obj.get() == object;
                         });
                         
    if (it != objects.end())
        objects.erase(it);
}

void Scene::ClearObjects()
{
    objects.clear();
}

void Scene::ClearLights()
{
    lights.clear();
}

int Scene::AddDefaultLight()
{
    std::unique_ptr<Light> defaultLight = std::make_unique<Light>();
    defaultLight->position = glm::vec3(5.0f, 5.0f, 5.0f);
    defaultLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
    defaultLight->intensity = 2.0f;
    
    lights.push_back(std::move(*defaultLight));
    return static_cast<int>(lights.size() - 1);
}

int Scene::AddLight(const Light& light)
{
    lights.push_back(light);
    return static_cast<int>(lights.size() - 1);
}

void Scene::RemoveLight(std::size_t index)
{
    if (index >= 0 && index < lights.size())
        lights.erase(lights.begin() + index);
}

Light* Scene::GetLight(std::size_t index)
{
    if (index >= 0 && index < lights.size())
        return &lights[index];
        
    return nullptr;
}

bool Scene::SaveToFile(const std::string& filepath)
{
    try
    {
        json sceneJson;
        
        // Save lights
        json lightsJson = json::array();
        for (const auto& light : lights)
        {
            json lightJson;
            lightJson["position"] = {light.position.x, light.position.y, light.position.z};
            lightJson["color"] = {light.color.r, light.color.g, light.color.b};
            lightJson["intensity"] = light.intensity;
            
            lightsJson.push_back(lightJson);
        }
        sceneJson["lights"] = lightsJson;
        
        // Save objects
        json objectsJson = json::array();
        for (const auto& object : objects)
        {
            if (!object)
                continue;
                
            json objectJson;
            objectJson["name"] = object->GetName();
            objectJson["visible"] = object->IsVisible();
            
            // Save transform
            objectJson["position"] = {object->GetPosition().x, object->GetPosition().y, object->GetPosition().z};
            objectJson["rotation"] = {object->GetRotation().x, object->GetRotation().y, object->GetRotation().z};
            objectJson["scale"] = {object->GetScale().x, object->GetScale().y, object->GetScale().z};
            
            // Save material
            const Material& material = object->GetMaterial();
            objectJson["material"]["ambient"] = {material.ambient.r, material.ambient.g, material.ambient.b};            objectJson["material"]["diffuse"] = {material.diffuse.r, material.diffuse.g, material.diffuse.b};
            objectJson["material"]["specular"] = {material.specular.r, material.specular.g, material.specular.b};
            objectJson["material"]["shininess"] = material.shininess;
            
            // Save shader information
            if (object->GetShader()) {
                objectJson["shader"] = object->GetShader()->GetName();
            }
            
            // Distinguish between primitives and custom models
            if (object->HasModel() && object->GetModel())
            {
                objectJson["objectType"] = "model";
                
                const std::string& modelPath = object->GetModel()->GetFilePath();
                objectJson["modelPath"] = modelPath;
                
                std::string modelName = "";
                std::filesystem::path path(modelPath);
                if (path.has_filename()) {
                    modelName = path.filename().replace_extension("").string();
                }
                objectJson["modelName"] = modelName;
            }
            else
            {
                objectJson["objectType"] = "primitive";
                
                std::string type = "Cube";
                if (object->GetName().find("Sphere") != std::string::npos)
                    type = "Sphere";
                else if (object->GetName().find("Plane") != std::string::npos)
                    type = "Plane";
                else if (object->GetName().find("Cylinder") != std::string::npos)
                    type = "Cylinder";
                else if (object->GetName().find("Cone") != std::string::npos)
                    type = "Cone";
                    
                objectJson["primitiveType"] = type;
            }
            
            objectsJson.push_back(objectJson);
        }
        sceneJson["objects"] = objectsJson;
        
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }
        
        file << std::setw(4) << sceneJson << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error saving scene to file: " << e.what() << std::endl;
        return false;
    }
}

bool Scene::LoadFromFile(const std::string& filepath)
{
    try
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for reading: " << filepath << std::endl;
            return false;
        }
        
        json sceneJson;
        file >> sceneJson;
        
        // Clear existing scene data
        objects.clear();
        lights.clear();
        
        // Load lights
        if (sceneJson.contains("lights") && sceneJson["lights"].is_array())
        {
            for (const auto& lightJson : sceneJson["lights"])
            {
                Light light;
                
                if (lightJson.contains("position") && lightJson["position"].is_array() && lightJson["position"].size() == 3)
                {
                    light.position.x = lightJson["position"][0];
                    light.position.y = lightJson["position"][1];
                    light.position.z = lightJson["position"][2];
                }
                
                if (lightJson.contains("color") && lightJson["color"].is_array() && lightJson["color"].size() == 3)
                {
                    light.color.r = lightJson["color"][0];
                    light.color.g = lightJson["color"][1];
                    light.color.b = lightJson["color"][2];
                }
                
                if (lightJson.contains("intensity") && lightJson["intensity"].is_number())
                {
                    light.intensity = lightJson["intensity"];
                }
                
                lights.push_back(light);
            }
        }
        
        // If no lights were loaded, add a default light
        if (lights.empty())
        {
            Light defaultLight;
            defaultLight.position = glm::vec3(5.0f, 5.0f, 5.0f);
            defaultLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
            defaultLight.intensity = 1.0f;
            lights.push_back(defaultLight);
        }

        // Load objects
        if (sceneJson.contains("objects") && sceneJson["objects"].is_array())
        {
            for (const auto& objectJson : sceneJson["objects"])
            {
                std::string name = "Object";
                if (objectJson.contains("name") && objectJson["name"].is_string())
                {
                    name = objectJson["name"];
                }
                
                std::unique_ptr<SceneObject> object;
                
                // Determine if this is a primitive or a custom model
                std::string objectType = "primitive";
                if (objectJson.contains("objectType") && objectJson["objectType"].is_string())
                {
                    objectType = objectJson["objectType"];
                }
                
                if (objectType == "model")
                {
                    std::string modelPath;
                    std::string modelName;
                    
                    if (objectJson.contains("modelPath") && objectJson["modelPath"].is_string())
                    {
                        modelPath = objectJson["modelPath"];
                    }
                    
                    if (objectJson.contains("modelName") && objectJson["modelName"].is_string())
                    {
                        modelName = objectJson["modelName"];
                    }
                    else if (!modelPath.empty())
                    {
                        // Extract model name from path if not provided
                        std::filesystem::path path(modelPath);
                        modelName = path.filename().replace_extension("").string();
                    }
                    
                    if (!modelPath.empty())
                    {
                        // Create a new SceneObject
                        object = std::make_unique<SceneObject>(name);
                        
                        // Normalize path for cross-platform compatibility
                        std::string normalizedPath = modelPath;
                        std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');
                        
                        // Load the model using ResourceManager
                        ResourceManager* resourceManager = ResourceManager::GetInstance();
                        Model* model = resourceManager->LoadModel(modelName, normalizedPath);
                        if (model)
                        {
                            object->SetModel(model);
                        }
                        else
                        {
                            std::cerr << "Failed to load model: " << normalizedPath << std::endl;
                            continue;
                        }
                    }
                    else
                    {
                        std::cerr << "Model path missing for object: " << name << std::endl;
                        continue; 
                    }
                }
                else
                {
                    // Handle primitive object
                    std::string primitiveType = "Cube"; 
                    
                    if (objectJson.contains("primitiveType") && objectJson["primitiveType"].is_string())
                    {
                        primitiveType = objectJson["primitiveType"];
                    }
                    else if (objectJson.contains("type") && objectJson["type"].is_string())
                    {
                        primitiveType = objectJson["type"];
                    }
                    
                    object = Primitives::CreatePrimitiveObject(primitiveType, name);
                    if (!object)
                        continue;
                }
                
                if (objectJson.contains("visible") && objectJson["visible"].is_boolean())
                {
                    object->SetVisible(objectJson["visible"]);
                }
                
                if (objectJson.contains("position") && objectJson["position"].is_array() && objectJson["position"].size() == 3)
                {
                    glm::vec3 position(
                        objectJson["position"][0],
                        objectJson["position"][1],
                        objectJson["position"][2]
                    );
                    object->SetPosition(position);
                }
                
                if (objectJson.contains("rotation") && objectJson["rotation"].is_array() && objectJson["rotation"].size() == 3)
                {
                    glm::vec3 rotation(
                        objectJson["rotation"][0],
                        objectJson["rotation"][1],
                        objectJson["rotation"][2]
                    );
                    object->SetRotation(rotation);
                }
                
                if (objectJson.contains("scale") && objectJson["scale"].is_array() && objectJson["scale"].size() == 3)
                {
                    glm::vec3 scale(
                        objectJson["scale"][0],
                        objectJson["scale"][1],
                        objectJson["scale"][2]
                    );
                    object->SetScale(scale);
                }
                
                if (objectJson.contains("material") && objectJson["material"].is_object())
                {
                    Material material = object->GetMaterial();
                    
                    if (objectJson["material"].contains("ambient") && objectJson["material"]["ambient"].is_array() && 
                        objectJson["material"]["ambient"].size() == 3)
                    {
                        material.ambient = glm::vec3(
                            objectJson["material"]["ambient"][0],
                            objectJson["material"]["ambient"][1],
                            objectJson["material"]["ambient"][2]
                        );
                    }
                    
                    if (objectJson["material"].contains("diffuse") && objectJson["material"]["diffuse"].is_array() && 
                        objectJson["material"]["diffuse"].size() == 3)
                    {
                        material.diffuse = glm::vec3(
                            objectJson["material"]["diffuse"][0],
                            objectJson["material"]["diffuse"][1],
                            objectJson["material"]["diffuse"][2]
                        );
                    }
                    
                    if (objectJson["material"].contains("specular") && objectJson["material"]["specular"].is_array() && 
                        objectJson["material"]["specular"].size() == 3)
                    {
                        material.specular = glm::vec3(
                            objectJson["material"]["specular"][0],
                            objectJson["material"]["specular"][1],
                            objectJson["material"]["specular"][2]
                        );
                    }
                    
                    if (objectJson["material"].contains("shininess") && objectJson["material"]["shininess"].is_number())
                    {
                        material.shininess = objectJson["material"]["shininess"];
                    }
                      object->SetMaterial(material);
                }
                
                // Load shader if specified
                if (objectJson.contains("shader") && objectJson["shader"].is_string()) {
                    std::string shaderName = objectJson["shader"];
                    ResourceManager* resourceManager = ResourceManager::GetInstance();
                    Shader* shader = resourceManager->GetShader(shaderName);
                    
                    // If specified shader does not exist, try to fall back to default
                    if (!shader) {
                        shader = resourceManager->GetShader("default");
                    }
                    
                    if (shader) {
                        object->SetShader(shader);
                    }
                }
                
                objects.push_back(std::move(object));
            }
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading scene from file: " << e.what() << std::endl;
        return false;
    }
}