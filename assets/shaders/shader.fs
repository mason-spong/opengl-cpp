#version 330 core

in vec3 vNormal;        // Received from vertex shader (for potential lighting later)
in vec2 vTexCoord;      // Received interpolated texture coordinates from vertex shader

out vec4 FragColor;     // Output color for the current pixel

uniform sampler2D textureSampler; // The texture sampler uniform

void main() {
    // Sample the texture at the interpolated texture coordinate
    vec4 textureColor = texture(textureSampler, vTexCoord);

    // Set the final fragment color to the color sampled from the texture
    FragColor = textureColor;

    // --- Optional: Combine with lighting later ---
    // Example: Simple ambient + diffuse based on normal (requires light direction uniform)
    // vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); // Example light direction
    // float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    // vec3 diffuse = diff * vec3(1.0); // White diffuse light
    // vec3 ambient = 0.2 * vec3(1.0); // Small amount of ambient light
    // FragColor = vec4(ambient + diffuse, 1.0) * textureColor;
}