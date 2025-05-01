cmake_minimum_required(VERSION 3.15)
project(OpenGLLearning VERSION 1.0.0)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add GLM_ENABLE_EXPERIMENTAL define globally
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# ImGui implementation files
set(IMGUI_IMPL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

# Source files
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Add ImGui implementation source files
list(APPEND SOURCE_FILES
    "${IMGUI_IMPL_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_IMPL_DIR}/imgui_impl_opengl3.cpp"
)

# Include directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${IMGUI_IMPL_DIR}"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    nlohmann_json::nlohmann_json
)

# Copy resources folder to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)