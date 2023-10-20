#ifndef BASICSPHEREMESH_H
#define BASICSPHEREMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include <string>
#include <vector>

struct basicSphereVertex
{
    glm::vec3 Position;
    glm::vec3 Colours;
    glm::vec2 Textures;
};

class basicSphereMesh {
public:
    std::vector<basicSphereVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

    basicSphereMesh(float radius, int segments)
    {
        generateSphereVertices(radius, segments);
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

    void generateSphereVertices(float radius, int segments)
    {
        // Create vertices and indices for the sphere
        // Clear any previous data
        vertices.clear();
        indices.clear();

        const float pi = glm::pi<float>();
        const float twoPi = 2.0f * pi;

        for (int lat = 0; lat <= segments; ++lat) {
            float theta = lat * pi / segments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int lon = 0; lon <= segments; ++lon) {
                float phi = lon * twoPi / segments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                glm::vec3 position = glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
                glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); // Set color as needed
                glm::vec2 textureCoords = glm::vec2(1.0f - (lon / static_cast<float>(segments)), 1.0f - (lat / static_cast<float>(segments)));

                vertices.push_back({ radius * position, color, textureCoords });
            }
        }

        // Generate indices for triangles
        for (int lat = 0; lat < segments; ++lat) {
            for (int lon = 0; lon < segments; ++lon) {
                int current = lat * (segments + 1) + lon;
                int next = current + segments + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(next);
                indices.push_back(next + 1);
                indices.push_back(current + 1);
            }
        }
    }

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(basicSphereVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Define vertex attributes (position, color, texture coordinates)
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(basicSphereVertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(basicSphereVertex), (void*)sizeof(glm::vec3));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(basicSphereVertex), (void*)(2 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
    }
};

#endif // !BASICSPHEREMESH_H
