#include "imgui_impl_glfw.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

// Simple implementation for demonstration purposes
// For full implementation, see: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_glfw.cpp

static GLFWwindow* g_Window = NULL;
static double g_Time = 0.0;

bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks)
{
    g_Window = window;
    g_Time = glfwGetTime();
    
    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendPlatformName = "imgui_impl_glfw";
    
    return true;
}

void ImGui_ImplGlfw_Shutdown()
{
    g_Window = NULL;
}

void ImGui_ImplGlfw_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    
    // Setup display size
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
    
    // Setup time step
    double current_time = glfwGetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;
}

void ImGui_ImplGlfw_InstallCallbacks(GLFWwindow* window) { }
void ImGui_ImplGlfw_RestoreCallbacks(GLFWwindow* window) { }
void ImGui_ImplGlfw_SetCallbacksChainForAllWindows(bool chain_for_all_windows) { }
void ImGui_ImplGlfw_WindowFocusCallback(GLFWwindow* window, int focused) { }
void ImGui_ImplGlfw_CursorEnterCallback(GLFWwindow* window, int entered) { }
void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) { }
void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) { }
void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { }
void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c) { }
void ImGui_ImplGlfw_MonitorCallback(GLFWmonitor* monitor, int event) { }