# Compiler
CXX := clang++

# Allow overriding build type: make BUILD_TYPE=debug (defaults to release)
BUILD_TYPE ?= release

# Directories (simple assignment so it's expanded at parse time)
SRC_DIR    := src
BUILD_DIR  := build/$(BUILD_TYPE)
OBJ_DIR    := $(BUILD_DIR)/obj
BIN_DIR    := $(BUILD_DIR)/bin

# Executable
TARGET_NAME := opengl_cube
TARGET_EXEC := $(BIN_DIR)/$(TARGET_NAME)

# Source → object mapping
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Flags (common + per-build‑type)
COMMON_CXXFLAGS  := -Wall -Wextra \
                    -I/opt/homebrew/opt/glfw/include \
                    -Ilibs -Iinclude \
                    -DGL_SILENCE_DEPRECATION \
                    -std=c++17
COMMON_LDFLAGS   := -L/opt/homebrew/opt/glfw/lib -lglfw
COMMON_FRAMEWORKS:= -framework OpenGL

ifeq ($(BUILD_TYPE),debug)
  BUILD_CXXFLAGS := -g -O0 -fno-omit-frame-pointer
  BUILD_DEFINES   :=
else
  BUILD_CXXFLAGS := -O2
  BUILD_DEFINES   := -DNDEBUG
endif

CXXFLAGS   := $(COMMON_CXXFLAGS) $(BUILD_CXXFLAGS) $(BUILD_DEFINES)
LDFLAGS    := $(COMMON_LDFLAGS)
FRAMEWORKS := $(COMMON_FRAMEWORKS)

# ——— PHONY targets ———
.PHONY: all debug release clean

all: $(TARGET_EXEC)

debug:
	@$(MAKE) BUILD_TYPE=debug all

release:
	@$(MAKE) BUILD_TYPE=release all

clean:
	@echo "Cleaning all build artifacts..."
	rm -rf build

# ——— Link the final executable ———
$(TARGET_EXEC): $(OBJS) | $(BIN_DIR)
	@echo "Linking $(BUILD_TYPE) build: $@"
	$(CXX) $(OBJS) $(LDFLAGS) $(FRAMEWORKS) -o $@

# ——— Compile each .cpp into .o ———
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $(BUILD_TYPE): $< → $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ——— Ensure directories exist ———
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Disable suffix rules
.SUFFIXES:
