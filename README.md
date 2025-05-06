# OpenGL Learning Application

An interactive educational desktop application for Windows that allows users to learn the basics of 3D graphics rendering using OpenGL. The application demonstrates key stages of the rendering pipeline: geometry processing, shaders, lighting, and object transformations.

## Features

- **Interactive 3D Scene**: Add, remove, and manipulate 3D objects in real-time
- **Basic 3D Shapes**: Includes cube, sphere, plane, cylinder, and cone primitives
- **Transform Controls**: Modify object transformations (translation, rotation, scaling)
- **Real-time Shader Editing**: Edit and compile GLSL shaders on the fly
- **Multiple Lighting Models**: Switch between Phong and Flat shading
- **Material Properties**: Adjust ambient, diffuse, and specular properties
- **Wireframe Mode**: Visualize the triangle mesh structure 
- **Scene Saving/Loading**: Save and load scene configurations to/from JSON files
- **Camera Controls**: Navigate the 3D scene using keyboard and mouse

## Development Progress

| Feature                   | Status      | Notes                                                      |
|---------------------------|-------------|-----------------------------------------------------------|
| 3D Scene Rendering        | ✅ Complete | Basic rendering pipeline working                           |
| Camera Navigation         | ✅ Complete | WASD + mouse controls working                              |
| Basic 3D Shapes           | ⏳ Partial  | Cube, Sphere implemented; others pending                   |
| Light Effects             | ✅ Complete | Basic lighting model implemented                           |
| UI Panels & Controls      | ✅ Complete | Fixed by using official ImGui implementation files         |
| Object Transformations    | ⏳ Pending  | UI controls exist but need testing                         |
| Material Editing          | ⏳ Pending  | UI controls exist but need testing                         |
| Shader Editor             | ⏳ Pending  | Basic infrastructure exists                                |
| Scene Save/Load           | ✅ Complete | Basic save/load dialogs with JSON processing               |

## Prerequisites

- Windows 10/11
- Visual Studio 2019 or newer
- CMake 3.15 or newer
- [vcpkg](https://github.com/microsoft/vcpkg) package manager

## Dependencies

- OpenGL 3.3+
- GLFW (window/context management)
- glad (OpenGL loader)
- GLM (3D math library)
- ImGui (UI library)
- nlohmann-json (JSON serialization)

## Build Instructions

### 1. Install vcpkg

If you don't have vcpkg installed:

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
```

### 2. Clone the Repository

```
git clone https://github.com/yourusername/OpenGLLearning.git
cd OpenGLLearning
```

### 3. Build with CMake

```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

Replace `[path-to-vcpkg]` with the actual path to your vcpkg installation.

### 4. Run the Application

```
cd Release
OpenGLLearning.exe
```

## Usage

### Camera Navigation

- **W/A/S/D**: Move camera forward/left/backward/right
- **Space/Ctrl**: Move camera up/down
- **Right Mouse Button + Mouse Move**: Rotate camera

### User Interface

- **Object Properties**: Select and modify 3D object properties
- **Scene Settings**: Adjust lighting and background color
- **Shader Editor**: Edit and compile GLSL shaders in real-time
- **Performance Overlay**: Monitor FPS and frame time

### Main Menu

- **File**: Create, open, and save scenes
- **Edit**: Access shader editor
- **View**: Toggle interface panels
- **Objects**: Add primitive shapes to the scene
- **Rendering**: Change rendering mode and lighting model

## Extending the Application

The application is designed to be easily extensible:

- Add new primitive shapes in the `Primitives` class
- Implement texture support by extending material properties
- Add alternative rendering backends (Vulkan/Direct3D)
- Implement shadow mapping for realistic lighting
- Add physics simulation

## Project Structure

- **include/**: Header files
- **src/**: Implementation files
- **resources/**: 
  - **shaders/**: GLSL shader files
  - **scenes/**: Saved scene configurations
  - **models/**: 3D model files

## Troubleshooting

### ImGui UI Not Displaying
If the ImGui UI elements aren't visible despite successful initialization:
1. Ensure you're using the official ImGui implementation files from the [ImGui repository](https://github.com/ocornut/imgui)
2. Check that both backends are properly initialized (ImGui_ImplGlfw_InitForOpenGL and ImGui_ImplOpenGL3_Init)
3. Verify that the OpenGL state is properly managed between your 3D rendering and ImGui rendering
4. Delete imgui.ini if it exists to allow ImGui to create a fresh configuration

### OpenGL Errors
If you encounter OpenGL-related errors:
1. Enable debug output using glEnable(GL_DEBUG_OUTPUT)
2. Check for OpenGL version compatibility
3. Ensure all shader compilation and linking is successful

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- OpenGL SuperBible
- Learn OpenGL by Joey de Vries
- GL Mathematics (GLM)
- Dear ImGui