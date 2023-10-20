#ifndef BASICCYLINDERMESH_H
#define BASICCYLINDERMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>

#include <vector>

struct basicCylinderVertex
{
    glm::vec3 Position;
    glm::vec3 Colours;
    glm::vec2 Textures;
};

class basicCylinderMesh {

public:
    std::vector<basicCylinderVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

    basicCylinderMesh(float radius, float height, int sides)
    {
        generateCylinderVertices(radius, height, sides);
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

    void generateCylinderVertices(float radius, float height, int sides)
    {
        for (int i = 1; i < sides+2; i++) {
            float theta = 2.0f * glm::pi<float>() * i / sides;
            float x = radius * cos(theta);
            float y = radius * sin(theta);

            // Vertices for the side of the cylinder
            basicCylinderVertex topVertex = { glm::vec3(x, y, height / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
            basicCylinderVertex bottomVertex = { glm::vec3(x, y, -height / 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) };

            vertices.push_back(topVertex);
            vertices.push_back(bottomVertex);

            // Indices for triangles
            indices.push_back(2 * i);
            indices.push_back(2 * i + 1);
            indices.push_back((2 * i + 2) % (2 * sides));
            indices.push_back(2 * i + 1);
            indices.push_back((2 * i + 3) % (2 * sides));
            indices.push_back(2 * i + 2);
        }
    }

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(basicCylinderVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(basicCylinderVertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(basicCylinderVertex), (void*)sizeof(glm::vec3));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(basicCylinderVertex), (void*)(2 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
    }
};

#endif // !BASICCYLINDERMESH_H
