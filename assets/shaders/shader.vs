#version 330 core // Use OpenGL version 3.3 with core profile

layout (location = 0) in vec3 aPos; // Input vertex position from attribute 0
layout (location = 1) in vec3 aColor; // Input vertex color from attribute 1

out vec3 vColor; // Output color to the fragment shader

uniform mat4 model; // Transformation matrix for object's position/rotation/scale
uniform mat4 view; // Transformation matrix for camera position/orientation
uniform mat4 projection; // Transformation matrix for perspective/orthographic view

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Calculate final vertex position
    vColor = aColor; // Pass color through
}
