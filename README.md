# opengl-cpp

This project is a step-by-step learning process to create a Minecraft-like graphics application using C++ and OpenGL, starting from the basics.

## How to Set Up/Build

This project uses Make for its build system and relies on the GLFW library for window creation and OpenGL context management.

### Prerequisites

* A C++ compiler (like Clang, which is standard on macOS).

* Make build tool (standard on macOS).

* [Homebrew](https://brew.sh/) package manager (recommended for installing GLFW).

### Installation

1. **Install Homebrew** (if you don't have it):
   Follow the instructions on the [Homebrew website](https://brew.sh/).

2. **Install GLFW using Homebrew**:
   Open your terminal and run:

   ```bash
   brew install glfw
   ```

   This will install the GLFW library and its header files to your Homebrew prefix (likely `/opt/homebrew/opt/glfw` or `/usr/local`).

### Building the Project

1. Navigate to the project directory in your terminal:

   ```bash
   cd /path/to/your/opengl-cpp
   ```

2. Run the `make` command:

   ```bash
   make
   ```

   The `Makefile` will compile the `main.cpp` file and link it with the installed GLFW library and the macOS OpenGL framework.

3. Upon successful compilation, an executable file named `opengl_cube` will be created in the project directory.

### Running the Project

1. In your terminal, from the project directory, run the executable:

   ```bash
   ./opengl_cube
   ```

   This should open a window with a dark cyan background.

### VS Code IntelliSense Setup (Optional)

If you are using VS Code with the C/C++ extension, you might see include errors (`#include errors detected`). To fix this and enable proper IntelliSense:

1. Open the Command Palette (`Cmd+Shift+P`).

2. Search for and select "C/C++: Edit Configurations (JSON)".

3. In the generated `.vscode/c_cpp_properties.json` file, add the path to your Homebrew GLFW include directory (e.g., `/opt/homebrew/opt/glfw/include`) to the `"includePath"` array within the relevant configuration.
