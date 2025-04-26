#version 330 core

in  vec3 vNormal;
out vec4 FragColor;

void main() {
    // Remap normals from [-1,1] to [0,1] for color‑coding:
    vec3 normalColor = normalize(vNormal) * 0.5 + 0.5;

    // Option A: show normals *instead of* your mesh color
    FragColor = vec4(normalColor, 1.0);
}
