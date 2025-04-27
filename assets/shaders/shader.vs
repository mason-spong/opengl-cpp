#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord; // Input texture coordinates
layout(location = 3) in float aLayerIndex; // New attribute

out vec3 vNormal;       // Pass normal to fragment shader
out vec2 vTexCoord;     // Pass texture coordinates to fragment shader
flat out float vLayerIndex; // New output (use 'flat'!)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Standard Model-View-Projection transformation
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Calculate world-space normal for lighting (if needed later)
    // Use the normal matrix to handle non-uniform scaling correctly
    vNormal = normalize(mat3(transpose(inverse(model))) * aNormal); // Corrected normal calc

    // Pass the texture coordinate directly to the fragment shader
    vTexCoord = aTexCoord;
    vLayerIndex = aLayerIndex; // Pass layer index through
}