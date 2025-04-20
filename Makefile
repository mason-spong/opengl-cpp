# Compiler
CXX = clang++

# Compiler flags:
# -Wall -Wextra: Enable common warnings
# -I/opt/homebrew/opt/glfw/include: Add Homebrew's actual GLFW include path
# -I.: Add the current directory to the include path so the compiler finds the 'glm' folder
# -DGL_SILENCE_DEPRECATION: Silence deprecation warnings for OpenGL functions on macOS
CXXFLAGS = -Wall -Wextra -I/opt/homebrew/opt/glfw/include -Ilibs -Iinclude -DGL_SILENCE_DEPRECATION -std=c++11

# Linker flags:
# -L/opt/homebrew/opt/glfw/lib: Add Homebrew's actual GLFW library path
# -lglfw: Link against the GLFW library
LDFLAGS = -L/opt/homebrew/opt/glfw/lib -lglfw 

# Frameworks:
# -framework OpenGL: Link against the macOS OpenGL framework
FRAMEWORKS = -framework OpenGL

# Target executable name
TARGET = opengl_cube

# Source files - Automatically find all .cpp files in the src directory
SRCS = $(wildcard src/*.cpp)

# Object files (generated from source files)
OBJS = $(SRCS:.cpp=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable from object files
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(FRAMEWORKS) -o $(TARGET)

# Rule to compile .cpp files into .o object files
# $<: the first prerequisite (the .cpp file)
# $@: the target (the .o file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target: remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
