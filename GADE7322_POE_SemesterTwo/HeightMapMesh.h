#ifndef HEIGHTMAPMESH_H
#define HEIGHTMAPMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

struct HeightMapVertex {
	glm::vec3 hMapVertices;
	//glm::vec3 colour;
	//glm::vec2 texture;
	glm::vec3 indices;
	unsigned int NUM_STRIPS;
	unsigned int NUM_VERTS_PER_STRIP;
};

class HeightMapMesh {
public:
	vector<HeightMapVertex> hMapVertices;
	vector<HeightMapVertex> indices;
	unsigned int NUM_STRIPS;
	unsigned int NUM_VERTS_PER_STRIP;

	unsigned int VAO;

	//constructor
	HeightMapMesh(vector <HeightMapVertex> hMapVertices, vector <HeightMapVertex> indices, int  NUM_STRIPS, int  NUM_VERTS_PER_STRIP) {
		this->hMapVertices = hMapVertices;
		this->indices = indices;
		this->NUM_STRIPS = NUM_STRIPS;
		this->NUM_VERTS_PER_STRIP;
		SetUpMesh();
	}

	void Draw(Shader& shader)
	{
		shader.use();
		glBindVertexArray(terrainVAO);

		for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
		{
			glDrawElements(GL_TRIANGLE_STRIP, NUM_VERTS_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * NUM_VERTS_PER_STRIP * strip));
		}

	}
private: 
	GLuint terrainVAO, terrainVBO, terrainEBO;
	void SetUpMesh() {
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, hMapVertices.size() * sizeof(float), &hMapVertices[0], GL_STATIC_DRAW);

		//pos attrin
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &terrainEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightMapVertex), (void*)offsetof(HeightMapVertex, position));
		glEnableVertexAttribArray(0);*/

		//colour
		/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(HeightMapVertex), (void*)offsetof(HeightMapVertex, colour));
		glEnableVertexAttribArray(1);*/

		/*glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HeightMapVertex), (void*)offsetof(HeightMapVertex, texture));
		glEnableVertexAttribArray(2);*/
	}
};
#endif
