#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "SceneObject.h"
#include "Shader.h"

class Primitives {
public:
    // Static methods to create primitive shapes (meshes only)
    static std::unique_ptr<Mesh> CreateCube(float size = 1.0f);
    static std::unique_ptr<Mesh> CreateSphere(float radius = 1.0f, unsigned int segments = 32);
    static std::unique_ptr<Mesh> CreatePlane(float width = 1.0f, float height = 1.0f);
    static std::unique_ptr<Mesh> CreateCylinder(float radius = 0.5f, float height = 2.0f, unsigned int segments = 32);
    static std::unique_ptr<Mesh> CreateCone(float radius = 0.5f, float height = 1.0f, unsigned int segments = 32);
    
    // Create complete primitive objects (including mesh, shader, and material)
    static std::unique_ptr<SceneObject> CreatePrimitiveObject(const std::string& type, const std::string& name);
    
private:
    
    // Helper method to set up default material
    static void SetupDefaultMaterial(SceneObject* object);
};