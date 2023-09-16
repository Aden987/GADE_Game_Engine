#ifndef BASICCUBEMESH_H
#define BASICCUBEMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

struct basicCubeVertex
{
	glm::vec3 position;
	//glm::vec3 colour;
	//glm::vec2 texture;
};

class basicCubeMesh
{
public:
	vector<basicCubeVertex> vertices;

	unsigned int VAO;

	//constructor
	basicCubeMesh(vector <basicCubeVertex> vertices) {
		this->vertices =  vertices;
		setUpMesh();
	}

	void Draw(Shader& shader)
	{
		shader.use();
		//shader.setVec3("colourIn", glm::vec3(0.5f, 0.0, 0.0f));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

private: 
	unsigned int VBO, EBO;
	void setUpMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(basicCubeVertex), &vertices[0], GL_DYNAMIC_DRAW);

		///ATtribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(basicCubeVertex), (void*)offsetof(basicCubeVertex, position));
		glEnableVertexAttribArray(0);

		//colour attri
		/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(basicCubeVertex), (void*)offsetof(basicCubeVertex, colour));
		glEnableVertexAttribArray(1);*/

		//texture pointers
		/*glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(basicCubeVertex), (void*)offsetof(basicCubeVertex, texture));
		glEnableVertexAttribArray(2);*/
	}
};

#endif // !BASICCUBEMESH_H

