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

glm::vec3 light_position = glm::vec3(0.0f, 23.0f, 0.0f);
glm::vec3 light_color = glm::vec3(0.7f, 0.7f, 0.6f);

Object::Object()
{
	object_position = glm::vec3(0, 0, 0);
	object_rotation = glm::vec3(0, 0, 0);
	object_scale = glm::vec3(0.1f, 0.1f, 0.1f);

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

void Object::Render(GLuint program, glm::mat4 view_matrix, glm::mat4 proj_matrix, glm::vec3 view_position)
{

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Do some translations, rotations and scaling
	glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), object_position);
	model_matrix = glm::rotate(model_matrix, object_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model_matrix = glm::rotate(model_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model_matrix = glm::rotate(model_matrix, object_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	model_matrix = glm::scale(model_matrix, object_scale);

	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &view_matrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	//-----------------------Lights
	glUniform3f(glGetUniformLocation(program, "lightPos"), light_position.x, light_position.y, light_position.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), light_color.x, light_color.y, light_color.z);

	glUniform3f(glGetUniformLocation(program, "viewPos"), view_position.x, view_position.y, view_position.z);

	mesh.Draw();

}

