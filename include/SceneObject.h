#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h" // Include the full Mesh definition

class Shader;

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
    
    // Transform functions
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
    
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetRotation() const { return m_Rotation; }
    glm::vec3 GetScale() const { return m_Scale; }
    
    glm::mat4 GetTransform();
    void SetTransform(const glm::mat4& transform);
    
    // Getters and setters
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }
    
    bool IsVisible() const { return m_Visible; }
    void SetVisible(bool visible) { m_Visible = visible; }
    
    Material& GetMaterial() { return m_Material; }
    void SetMaterial(const Material& material) { m_Material = material; }
    
    Mesh* GetMesh() const { return m_Mesh.get(); }
    void SetMesh(std::unique_ptr<Mesh> mesh) { m_Mesh = std::move(mesh); }
    
    Shader* GetShader() const { return m_Shader; }
    void SetShader(Shader* shader) { m_Shader = shader; }
    
protected:
    std::string m_Name;
    bool m_Visible = true;
    
    // Transform properties
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    glm::mat4 m_Transform = glm::mat4(1.0f);
    bool m_TransformDirty = false;
    
    // Rendering properties
    Material m_Material;
    std::unique_ptr<Mesh> m_Mesh;
    Shader* m_Shader = nullptr; // Not owned
};