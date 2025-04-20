#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "World.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

Renderer::Renderer(const Mesh &mesh, const Shader &shader) : meshToDraw(mesh), shaderToUse(shader)
{
    // Renderer constructor can set up global GL state if needed
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(const World &world, const Camera &camera)
{
    // Clear buffers
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    shaderToUse.use();

    // Set view and projection matrices (these are usually per-frame, not per-object)
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    shaderToUse.setMatrix4("view", view);
    shaderToUse.setMatrix4("projection", projection);

    // Bind the mesh (can be done once if all objects use the same mesh)
    meshToDraw.bind();

    // Iterate through objects in the world and draw them
    const auto &blocks = world.getBlocksToRender();
    for (const auto &blockPos : blocks)
    {
        // Calculate the model matrix for this specific block
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, blockPos); // Move the block to its world position
        // Add rotation or scale here if blocks could be rotated/scaled individually

        // Pass the model matrix to the shader
        shaderToUse.setMatrix4("model", model);

        // Draw the mesh using the bound VAO and active shader
        glDrawElements(GL_TRIANGLES, meshToDraw.indexCount, GL_UNSIGNED_INT, 0);
    }

    meshToDraw.unbind(); // Unbind mesh after drawing
    glUseProgram(0);     // Unbind shader after drawing
}