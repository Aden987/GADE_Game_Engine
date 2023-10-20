#ifndef BASICCONEMESH_H
#define BASICCONEMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>
#include <vector>

struct basicConeVertex
{
    glm::vec3 Position;
    glm::vec3 Colours;
    glm::vec2 Textures;
};

class basicConeMesh {
public:
    std::vector<basicConeVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

    basicConeMesh(float radius, float height, int sides)
    {
        generateConeVertices(radius, height, sides);
        setupMesh();
    }

    void Draw(Shader& shader)
    {
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO, EBO;

    void generateConeVertices(float radius, float height, int sides)
    {
        // Create vertices and indices for the cone
        // Clear any previous data
        vertices.clear();
        indices.clear();

        // Vertex at the tip of the cone
        glm::vec3 tipVertex = { 0.0f, 0.0f, height / 2.0f };
        vertices.push_back({ tipVertex, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 0.5f) });

        // Vertices for the base of the cone
        for (int i = 0; i < sides; ++i) {
            float theta = 2.0f * glm::pi<float>() * i / sides;
            float x = radius * cos(theta);
            float y = radius * sin(theta);

            glm::vec3 baseVertex = { x, y, -height / 2.0f };
            vertices.push_back({ baseVertex, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(x / radius, y / radius) });
        }

        // Indices for triangles
        for (int i = 1; i < sides; ++i) {
            indices.push_back(0);  // Tip vertex
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        indices.push_back(0);  // Wrap around to connect the last vertex to the first
        indices.push_back(sides);
        indices.push_back(1);

        // Initialize the vertices and indices for the cone
    }

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(basicConeVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Define vertex attributes (position, color, texture coordinates)
         // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(basicConeVertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(basicConeVertex), (void*)sizeof(glm::vec3));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(basicConeVertex), (void*)(2 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
    }
};

#endif // !BASICCONEMESH_H

