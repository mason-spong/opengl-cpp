#version 330 core

in vec3 vNormal;        // Received from vertex shader (for potential lighting later)
in vec2 vTexCoord;      // Received interpolated texture coordinates from vertex shader
flat in float vLayerIndex; // Receive layer index (flat)

out vec4 FragColor;     // Output color for the current pixel

uniform sampler2DArray textureSampler; // Use sampler2DArray

void main() {
    // Sample using 3D coordinate (U, V, Layer)
    vec4 textureColor = texture(textureSampler, vec3(vTexCoord, vLayerIndex));

    // Optional lighting...
    // FragColor = textureColor * lightingFactor;
    FragColor = textureColor;
}