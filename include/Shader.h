#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>

// Helper function to read shader source code from a file
std::string readShaderFile(const std::string &filePath);

// Helper function to compile shaders
unsigned int compileShader(unsigned int type, const char *source);

// Represents a compiled and linked shader program
class Shader
{
public:
    unsigned int ID; // The OpenGL shader program ID

    // Constructor reads, compiles, and links shaders
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    // Destructor to clean up the shader program
    ~Shader();

    // Use the shader program
    void use() const;

    // Utility uniform functions (add more as needed)
    void setMatrix4(const std::string &name, const glm::mat4 &matrix) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;
};

#endif