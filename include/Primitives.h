#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Mesh.h" // Include the full Mesh definition instead of forward declaration

class Primitives {
public:
    // Static methods to create primitive shapes
    static std::unique_ptr<Mesh> CreateCube(float size = 1.0f);
    static std::unique_ptr<Mesh> CreateSphere(float radius = 1.0f, unsigned int segments = 32);
    static std::unique_ptr<Mesh> CreatePlane(float width = 1.0f, float height = 1.0f);
    static std::unique_ptr<Mesh> CreateCylinder(float radius = 0.5f, float height = 2.0f, unsigned int segments = 32);
    static std::unique_ptr<Mesh> CreateCone(float radius = 0.5f, float height = 1.0f, unsigned int segments = 32);
    
private:
    // Helper methods for UV calculations
    static glm::vec2 CalculateSphereUV(const glm::vec3& point);
};