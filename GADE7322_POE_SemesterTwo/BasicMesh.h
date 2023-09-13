#ifndef BASICMESH_H
#define BASICMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

struct BasicVertex {
	//position atr
	glm::vec3 Position;
	//colour atr
	glm::vec3 Colours;
	//texture
	glm::vec2 Tex;
};

class BasicMesh {
public:
	vector<BasicVertex> vertices;

	unsigned int VAO;

	BasicMesh(vector<BasicVertex> vertices)
	{
		this->vertices = vertices;

		setupMesh();
	}

	void Draw(Shader& shader)
	{
		shader.use();
		//shader.setVec3("colourIn", glm::vec3(1.0f, 0.0f, 0.0f));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
private:
	unsigned int VBO,EBO;
	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(BasicVertex), &vertices[0], GL_DYNAMIC_DRAW);

		///ATtribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)offsetof(BasicVertex, Position));
		glEnableVertexAttribArray(0);

		//colour attri
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)offsetof(BasicVertex, Colours));
		glEnableVertexAttribArray(1);

		//texture pointers
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)offsetof(BasicVertex, Tex));
		glEnableVertexAttribArray(2);
	}
};


#endif

