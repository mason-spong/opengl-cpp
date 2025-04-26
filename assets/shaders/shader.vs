#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // standard MVP
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // To correctly transform normals under non‑uniform scale, use the normal matrix:
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vNormal = normalize(normalMatrix * aNormal);
}
