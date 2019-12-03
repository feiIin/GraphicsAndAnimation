#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Mesh.h"


class Object
{
public:
	Object();
	void LoadObject(string meshName);
	void LoadTextures(string texName);
	//SEND POSITIONS ETC TO SHADER
	void Render(GLuint program, glm::mat4 viewMatrix, glm::mat4 proj_matrix);

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
private:
	Mesh mesh;
	GLuint texture;
};