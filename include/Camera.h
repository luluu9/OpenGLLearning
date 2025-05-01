#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
    ~Camera() = default;

    void ProcessInput(GLFWwindow* window, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void SetPosition(const glm::vec3& position) { m_Position = position; UpdateViewMatrix(); }
    glm::vec3 GetPosition() const { return m_Position; }

    void SetFOV(float fov) { m_FOV = fov; UpdateProjectionMatrix(); }
    float GetFOV() const { return m_FOV; }

    void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; UpdateProjectionMatrix(); }
    float GetAspectRatio() const { return m_AspectRatio; }

    void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; UpdateProjectionMatrix(); }
    float GetNearPlane() const { return m_NearPlane; }

    void SetFarPlane(float farPlane) { m_FarPlane = farPlane; UpdateProjectionMatrix(); }
    float GetFarPlane() const { return m_FarPlane; }

    void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
    float GetMovementSpeed() const { return m_MovementSpeed; }

    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
    float GetMouseSensitivity() const { return m_MouseSensitivity; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    // Camera position and orientation
    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Euler angles
    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;

    // Projection parameters
    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    // Camera options
    float m_MovementSpeed = 2.5f;
    float m_MouseSensitivity = 0.1f;

    // Cached matrices
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

    // Input tracking
    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;
};