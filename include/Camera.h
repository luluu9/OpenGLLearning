#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
    ~Camera() = default;

    void ProcessInput(GLFWwindow* window, float deltaTime, bool ignoreKeyboardInput, bool ignoreMouseInput);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void SetPosition(const glm::vec3& newPosition) { position = newPosition; UpdateViewMatrix(); }
    glm::vec3 GetPosition() const { return position; }

    void SetFOV(float newFov) { fov = newFov; UpdateProjectionMatrix(); }
    float GetFOV() const { return fov; }

    void SetAspectRatio(float newAspectRatio) { aspectRatio = newAspectRatio; UpdateProjectionMatrix(); }
    float GetAspectRatio() const { return aspectRatio; }

    void SetNearPlane(float newNearPlane) { nearPlane = newNearPlane; UpdateProjectionMatrix(); }
    float GetNearPlane() const { return nearPlane; }

    void SetFarPlane(float newFarPlane) { farPlane = newFarPlane; UpdateProjectionMatrix(); }
    float GetFarPlane() const { return farPlane; }

    void SetMovementSpeed(float speed) { movementSpeed = speed; }
    float GetMovementSpeed() const { return movementSpeed; }

    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    float GetMouseSensitivity() const { return mouseSensitivity; }

    void SetIgnoreScrollInput(bool ignore) { ignoreScrollInput = ignore; }
    bool GetIgnoreScrollInput() const { return ignoreScrollInput; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    // Camera position and orientation
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Euler angles
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Projection parameters
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    // Camera options
    float movementSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    bool ignoreScrollInput = false;

    // Cached matrices
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    // Input tracking
    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;
};