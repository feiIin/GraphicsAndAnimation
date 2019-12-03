#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Object.h"
#include "Mesh.h"
#include <glm\gtx\transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// --------- create lights

glm::vec3 LightPosition = glm::vec3(0.0f, 23.0f, 0.0f);
glm::vec3 LightColour = glm::vec3(0.5f, 0.5f, 0.5f);
//glm::vec3 ObjectColour = glm::vec3(1.0f, 1.0f, 1.0f);
// create new light for object lamp

Object::Object()
{
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(0.1f, 0.1f, 0.1f);

}

void Object::LoadObject(string meshName)
{
	mesh.LoadModel(meshName);
}


//---------- Method to load the texture of my objects
void Object::LoadTextures(string texName)
{
	// Load Texture OPENGL 4.3
	string name = texName;
	glGenTextures(1, &texture);
	stbi_set_flip_vertically_on_load(true);

	// Load image Information.
	int iWidth, iHeight, iChannels;
	unsigned char* iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming operations now have effect on this texture object

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, iWidth, iHeight);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGBA, GL_UNSIGNED_BYTE, iData);

	// This only works for 2D Textures...
	// Set the texture wrapping parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters (next lecture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps (next lecture)
	glGenerateMipmap(GL_TEXTURE_2D);


}

void Object::Render(GLuint program, glm::mat4 viewMatrix, glm::mat4 proj_matrix, glm::vec3 viewPos)
{

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Do some translations, rotations and scaling
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	modelMatrix = glm::scale(modelMatrix, scale);

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	//-----------------------Lights
	glUniform3f(glGetUniformLocation(program, "lightPos"), LightPosition.x, LightPosition.y, LightPosition.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), LightColour.x, LightColour.y, LightColour.z);
	//glUniform3f(glGetUniformLocation(program, "objectColor"), ObjectColour.x, ObjectColour.y, LightPosition.z);

	glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	mesh.Draw();

}

