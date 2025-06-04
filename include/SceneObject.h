#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "Model.h"

class Shader;
class Camera;

struct Material {
    glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
    float shininess = 32.0f;
};

class SceneObject {
public:
    SceneObject(const std::string& name);
    virtual ~SceneObject();
    
    virtual void Update(float deltaTime);
    virtual void Draw();
    virtual void DrawHighlight(Camera* camera);
    
    // Transform functions
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
    
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetRotation() const { return rotation; }
    glm::vec3 GetScale() const { return scale; }
    
    glm::mat4 GetTransform();
    void SetTransform(const glm::mat4& transform);
    
    // Getters and setters
    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    
    bool IsVisible() const { return visible; }
    void SetVisible(bool isVisible) { visible = isVisible; }
    
    Material& GetMaterial() { return material; }
    void SetMaterial(const Material& newMaterial) { material = newMaterial; }
    
    Mesh* GetMesh() const { return mesh.get(); }
    void SetMesh(std::unique_ptr<Mesh> newMesh) { mesh = std::move(newMesh); model = nullptr; }
    
    Model* GetModel() const { return model; }
    void SetModel(Model* newModel) { model = newModel; mesh.reset(); }
    Shader* GetShader() const { return shader; }
    void SetShader(Shader* newShader) { shader = newShader; }
    
    bool IsHighlighted() const { return highlighted; }
    void SetHighlighted(bool isHighlighted) { highlighted = isHighlighted; }
    
    bool HasModel() const { return model != nullptr; }
    
protected:
    std::string name;
    bool visible = true;
    
    // Transform properties
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 transform = glm::mat4(1.0f);
    bool transformDirty = false;
      // Rendering properties
    Material material;
    std::unique_ptr<Mesh> mesh;
    Shader* shader = nullptr;
    Model* model = nullptr;
    bool highlighted = false;
};