#version 330 core // Use OpenGL version 3.3 with core profile

in vec3 vColor; // Input color from the vertex shader (interpolated)

out vec4 FragColor; // Output final color for the pixel

void main()
{
    FragColor = vec4(vColor, 1.0); // Set the pixel color
}
