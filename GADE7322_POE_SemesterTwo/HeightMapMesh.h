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
	glm::vec3 position;
	//glm::vec3 colour;
	glm::vec2 texture;
};

class HeightMapMesh {
public:
	vector<HeightMapVertex> hMapVertices;

	unsigned int VAO;

	//constructor
	HeightMapMesh(vector <HeightMapVertex> hMapVertices) {
		this->hMapVertices = hMapVertices;
		SetUpMesh();
	}

	void Draw(Shader& shader)
	{
		shader.use();
	}
private: 
	GLuint terrainVAO, terrainVBO, terrainEBO;
	void SetUpMesh() {
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);

		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, hMapVertices.size() * sizeof(HeightMapVertex), &hMapVertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightMapVertex), (void*)offsetof(HeightMapVertex, position));
		glEnableVertexAttribArray(0);

		//colour
		/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(HeightMapVertex), (void*)offsetof(HeightMapVertex, colour));
		glEnableVertexAttribArray(1);*/

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HeightMapVertex), (void*)offsetof(HeightMapVertex, texture));
		glEnableVertexAttribArray(2);
	}
};
#endif
