#include "Camera.h"
#include <algorithm>
#include <iostream>

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    glfwSetScrollCallback(glfwGetCurrentContext(), [](GLFWwindow* window, double xoffset, double yoffset) {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (camera && !camera->GetIgnoreScrollInput())
            camera->ProcessMouseScroll(static_cast<float>(yoffset));
    });
    glfwSetWindowUserPointer(glfwGetCurrentContext(), this);
}

void Camera::ProcessInput(GLFWwindow* window, float deltaTime, bool ignoreKeyboardInput, bool ignoreMouseInput)
{
    if (ignoreMouseInput)
        SetIgnoreScrollInput(true);
    else
        SetIgnoreScrollInput(false);

    // Only process keyboard input if UI is not capturing it
    if (ignoreKeyboardInput)
        return;

    float velocity = movementSpeed * deltaTime;

    // Forward/backward movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    
    // Left/right movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    
    // Up/down movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += worldUp * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position -= worldUp * velocity;

    
    // Handle mouse input
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    // Only rotate camera if right mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos); // Reversed since y-coordinates range from bottom to top
        
        ProcessMouseMovement(xoffset, yoffset);
    }
    
    lastX = xpos;
    lastY = ypos;
    
    // Update the view matrix after processing all inputs
    UpdateViewMatrix();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    // Constrain pitch to avoid flipping
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    
    // Update camera vectors
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    front = glm::normalize(newFront);
    // Recalculate right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::ProcessMouseScroll(float yoffset)
{
    // Adjust fov for zoom effect
    fov -= yoffset;
    fov = std::clamp(fov, 1.0f, 90.0f);
    
    UpdateProjectionMatrix();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return projectionMatrix;
}

void Camera::UpdateViewMatrix()
{
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::UpdateProjectionMatrix()
{
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}