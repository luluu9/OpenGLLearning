#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

struct Light {
    glm::vec3 position = glm::vec3(0.0f, 10.0f, 10.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};

class SceneObject;

class Scene {
public:
    Scene();
    ~Scene();
    
    void Update(float deltaTime);
    
    // Object management
    SceneObject* AddObject(std::unique_ptr<SceneObject> object);
    void RemoveObject(SceneObject* object);
    void ClearObjects();
    
    // Accessor methods
    const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const { return objects; }
    const std::vector<Light>& GetLights() const { return lights; }
    
    // Light management
    void ClearLights();
    int AddDefaultLight();
    int AddLight(const Light& light);
    void RemoveLight(int index);
    Light* GetLight(int index);
    
    // Scene serialization
    bool SaveToFile(const std::string& filepath);
    bool LoadFromFile(const std::string& filepath);
    
private:
    std::vector<std::unique_ptr<SceneObject>> objects;
    std::vector<Light> lights;
};