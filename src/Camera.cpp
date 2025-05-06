#include "Camera.h"
#include <algorithm>
#include <iostream>

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
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

    float velocity = m_MovementSpeed * deltaTime;

    // Forward/backward movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Position += m_Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Position -= m_Front * velocity;
    
    // Left/right movement
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Position -= m_Right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Position += m_Right * velocity;
    
    // Up/down movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_Position += m_WorldUp * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_Position -= m_WorldUp * velocity;

    
    // Handle mouse input
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }
    
    // Only rotate camera if right mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        float xoffset = static_cast<float>(xpos - m_LastX);
        float yoffset = static_cast<float>(m_LastY - ypos); // Reversed since y-coordinates range from bottom to top
        
        ProcessMouseMovement(xoffset, yoffset);
    }
    
    m_LastX = xpos;
    m_LastY = ypos;
    
    // Update the view matrix after processing all inputs
    UpdateViewMatrix();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;
    
    m_Yaw += xoffset;
    m_Pitch += yoffset;
    
    // Constrain pitch to avoid flipping
    m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
    
    // Update camera vectors
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    
    m_Front = glm::normalize(front);
    // Recalculate right and up vectors
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::ProcessMouseScroll(float yoffset)
{
    // Adjust FOV for zoom effect
    m_FOV -= yoffset;
    m_FOV = std::clamp(m_FOV, 1.0f, 90.0f);
    
    UpdateProjectionMatrix();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return m_ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

void Camera::UpdateViewMatrix()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}