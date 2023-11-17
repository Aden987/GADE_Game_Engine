#ifndef LIGHTING_H
#define LIGHTING_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

struct lightingVertex {
	glm::vec3 Position;
};
class basicLighting {
public:
	vector<lightingVertex> vertices;
	unsigned VAO;

	//constructor
	basicLighting(vector<lightingVertex> vertices)
	{
		this->vertices = vertices;
		SetUpLighting();
	}
	void Draw(Shader& shader)
	{
		shader.use();
		//shader.setVec3("colourIn", glm::vec3(1.0f, 0.0f, 0.0f));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
private:
	unsigned int EBO, lightVAO;
	void SetUpLighting()
	{
		glGenVertexArrays(1, &lightVAO);
		glGenVertexArrays(1, &EBO);
		glBindVertexArray(lightVAO);
		// we only need to bind to the VBO, the container's VBO's data already contains the data.
		glBindBuffer(GL_ARRAY_BUFFER, EBO);
		// set the vertex attribute 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)offsetof(lightingVertex, Position));
		glEnableVertexAttribArray(0);
	}
};
#endif
