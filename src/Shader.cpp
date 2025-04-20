#include "Shader.h"

// --- Helper Functions Implementation ---

std::string readShaderFile(const std::string &filePath)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;
    std::string shaderCode;

    // Ensure ifstream objects can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open file
        shaderFile.open(filePath);
        // Read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // Close file handlers
        shaderFile.close();
        // Convert stream into string
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
        return ""; // Return empty string on failure
    }
    return shaderCode;
}

unsigned int compileShader(unsigned int type, const char *source)
{
    unsigned int id = glCreateShader(type); // Create a shader object (ID)
    glShaderSource(id, 1, &source, NULL);   // Set the shader source code
    glCompileShader(id);                    // Compile the shader

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        if (type == GL_VERTEX_SHADER)
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
        else if (type == GL_FRAGMENT_SHADER)
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n";
        else
            std::cerr << "ERROR::SHADER::UNKNOWN_TYPE::COMPILATION_FAILED\n";
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
        glDeleteShader(id); // Delete the shader if compilation failed
        return 0;
    }
    return id; // Return the shader ID
}

// --- Shader Class Implementation ---

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    // Read shader files
    std::string vertexCode = readShaderFile(vertexPath);
    std::string fragmentCode = readShaderFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty())
    {
        ID = 0; // Indicate failure
        return;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile shaders
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

    if (vertex == 0 || fragment == 0)
    {
        // compilation failed, helper prints error
        glDeleteShader(vertex);   // Safe to call on 0
        glDeleteShader(fragment); // Safe to call on 0
        ID = 0;                   // Indicate failure
        return;
    }

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << infoLog << std::endl;
        glDeleteProgram(ID);
        ID = 0; // Indicate failure
    }

    // Delete the shaders as they're linked into our program now and no longer needed
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    if (ID != 0)
    { // Only delete if the program was successfully created
        glDeleteProgram(ID);
    }
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setMatrix4(const std::string &name, const glm::mat4 &matrix) const
{
    // Get the location of the uniform variable in the shader
    // We get the location every time here for simplicity, but for performance
    // you would typically get all uniform locations once after linking and store them.
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    if (uniformLocation == -1)
    {
        // Optional: Add a warning if the uniform is not found
        // std::cerr << "Warning: Uniform '" << name << "' not found in shader program " << ID << std::endl;
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

// Implementations for other uniform setters (copied from common practice)
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}