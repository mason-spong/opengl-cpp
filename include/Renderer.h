#ifndef RENDERER_H
#define RENDERER_H

#include "Mesh.h"
#include "Shader.h"
#include "World.h"
#include "Camera.h"

// Handles the rendering process
class Renderer
{
private:
    const Mesh &meshToDraw;    // Reference to the shared mesh (e.g., cube mesh)
    const Shader &shaderToUse; // Reference to the shared shader

public:
    Renderer(const Mesh &mesh, const Shader &shader);

    void render(const World &world, const Camera &camera);
};

#endif