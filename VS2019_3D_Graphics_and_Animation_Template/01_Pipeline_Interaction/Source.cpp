// 3D Graphics and Animation - Main Template
// Visual Studio 2019
// Last Changed 01/10/2019

#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <GL/glew.h>			// Add library to extend OpenGL to newest version
#include <GLFW/glfw3.h>			// Add library to launch a window
#include <GLM/glm.hpp>			// Add helper maths library
#include <GLM/gtx/transform.hpp>

//#include <stb_image.h>			// Add library to load images for textures

#include "Mesh.h"				// Simplest mesh holder and OBJ loader 
#include "Object.h"


// MAIN FUNCTIONS
void setupRender();
void startup();
void update(GLfloat currentTime);
void render(GLfloat currentTime);
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
string readShader(string name);
void checkErrorShader(GLuint shader);
void errorCallbackGLFW(int error, const char* description);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);

// CALLBACK FUNCTIONS FOR WINDOW
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);


// VARIABLES
GLFWwindow* window;											// Keep track of the window
int				windowWidth = 640;
int				windowHeight = 480;
bool			running = true;									// Are we still running?
glm::mat4		proj_matrix;									// Projection Matrix
glm::vec3		camera_position = glm::vec3(0.075f, 0.430f, 1.0f);	
glm::vec3		camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;
GLuint			program;
GLint			proj_location;
glm::vec3		modelPosition;
glm::vec3		modelRotation;
Object princess; // ------- create objects
Object room;
Object bed;
Object dinosaur;
Object lights;
Object book;
Object mattress;
Object pillows;
Object banano;
Object night;

// Varibles for FRAMEBUFFER
GLuint			framebuffer;
GLuint			framebufferTexture;
GLuint			depthbuffer;
GLuint			displayVao;
GLuint			displayBuffer[2];
std::vector < glm::vec2 > displayVertices;
std::vector < glm::vec2 > displayUvs;
GLuint			displayProgram;


// FPS camera variables
GLfloat			yaw = -90.0f;	// init pointing to inside
GLfloat			pitch = 0.0f;	// start centered
GLfloat			lastX = windowWidth / 2.0f;	// start middle screen
GLfloat			lastY = windowHeight / 2.0f;	// start middle screen
bool			firstMouse = true;


int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "My OpenGL Application";

	// Fullscreen
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).




	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {

	// ----------Start Framebuffer Object---------------
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create a texture for the framebuffer
	glGenTextures(1, &framebufferTexture);

	// Bind this texture so we can modify it.
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);

	//  Start the texture and give it a size but no data- of course if you resize you need to change your texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// filtering needed 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Depth buffer texture	- Need to attach depth too otherwise depth testing will not be performed.
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	// Create a quad to display our framebuffer
	displayVertices.push_back(glm::vec2(-1.0f, 1.0f));
	displayVertices.push_back(glm::vec2(-1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(-1.0f, 1.0f));
	displayVertices.push_back(glm::vec2(1.0f, -1.0f));
	displayVertices.push_back(glm::vec2(1.0f, 1.0f));

	displayUvs.push_back(glm::vec2(0.0f, 1.0f));
	displayUvs.push_back(glm::vec2(0.0f, 0.0f));
	displayUvs.push_back(glm::vec2(1.0f, 0.0f));
	displayUvs.push_back(glm::vec2(0.0f, 1.0f));
	displayUvs.push_back(glm::vec2(1.0f, 0.0f));
	displayUvs.push_back(glm::vec2(1.0f, 1.0f));

	glGenBuffers(2, displayBuffer);        // Create a new buffer   
	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, displayVertices.size() * sizeof(glm::vec2), &displayVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, displayUvs.size() * sizeof(glm::vec2), &displayUvs[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &displayVao);        // Create Vertex Array Object
	glBindVertexArray(displayVao);        // Bind VertexArrayObject

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // vertices
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); //uv
	glEnableVertexAttribArray(1);

	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//load shaders for framebuffer displays
	displayProgram = glCreateProgram();

	string dvs_text = readShader("vs_display.glsl"); const char* dvs_source = dvs_text.c_str();
	GLuint dvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(dvs, 1, &dvs_source, NULL);
	glCompileShader(dvs);
	checkErrorShader(dvs);
	glAttachShader(displayProgram, dvs);

	string dfs_text = readShader("fs_display.glsl"); const char* dfs_source = dfs_text.c_str();
	GLuint dfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(dfs, 1, &dfs_source, NULL);
	glCompileShader(dfs);
	checkErrorShader(dfs);
	glAttachShader(displayProgram, dfs);

	glLinkProgram(displayProgram);
	glUseProgram(displayProgram);


	princess.LoadObject("banana.obj");
	princess.LoadTextures("bananaAI-o.png");
	princess.object_position = glm::vec3(0.075f, 0.230f, -0.25f);

	room.LoadObject("room3.obj");
	room.LoadTextures("room.png");

	bed.LoadObject("bed.obj");
	bed.LoadTextures("wood1.png");

	dinosaur.LoadObject("dinodino.obj");
	dinosaur.LoadTextures("dinodino.png");

	lights.LoadObject("lights.obj");
	lights.LoadTextures("lampade.png");

	book.LoadObject("book.obj");
	book.LoadTextures("book.png");
	book.object_position = glm::vec3(0.05f, 0.265f, -0.35f);
	book.object_rotation.y = 0.0f;

	mattress.LoadObject("mattress.obj");
	mattress.LoadTextures("bed1.png");

	pillows.LoadObject("pillows.obj");
	pillows.LoadTextures("bedreversed.png");

	banano.LoadObject("banano.obj");
	banano.LoadTextures("banano.png");

	night.LoadObject("night.obj");
	night.LoadTextures("night.png");

	program = glCreateProgram();

	string vs_text = readShader("vs_model.glsl"); const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader(program, vs);

	string fs_text = readShader("fs_model.glsl"); const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glUseProgram(program);

	// Start from the centre
	modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

bool increasing;
void update(GLfloat currentTime)
{
	book.object_rotation.y += 0.01f;

	if (keyStatus[GLFW_KEY_RIGHT])			princess.object_position.z -= 0.1f * deltaTime;
	if (keyStatus[GLFW_KEY_LEFT])			princess.object_position.z += 0.1f * deltaTime;
	if (keyStatus[GLFW_KEY_UP])				princess.object_position.x -= 0.1f * deltaTime;
	if (keyStatus[GLFW_KEY_DOWN])			princess.object_position.x += 0.1f * deltaTime;

	if (keyStatus[GLFW_KEY_J])				dinosaur.object_position.z -= 0.1f * deltaTime;
	if (keyStatus[GLFW_KEY_L])				dinosaur.object_position.z += 0.1f * deltaTime;
	if (keyStatus[GLFW_KEY_K])				dinosaur.object_position.x -= 0.1f * deltaTime;
	if (keyStatus[GLFW_KEY_I])				dinosaur.object_position.x += 0.1f * deltaTime;
	//	if (keyStatus[GLFW_KEY_KP_ADD])			bananoTemo.scale.z += 0.10f;
	//	if (keyStatus[GLFW_KEY_KP_SUBTRACT])	bananoTemp.scale.z -= 0.10f;
	// calculate movement
	GLfloat camera_speed = .5f * deltaTime;
	
	if (keyStatus[GLFW_KEY_W]) camera_position += camera_speed * camera_front;
	if (keyStatus[GLFW_KEY_S]) camera_position -= camera_speed * camera_front;
	if (keyStatus[GLFW_KEY_A]) camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
	if (keyStatus[GLFW_KEY_D]) camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
}

void render(GLfloat currentTime) {
	//===========RENDER FRAMEBUFFER TO TEXTURE===============

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, windowWidth, windowHeight);

	//Clear colour and deep buffer
	glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(program);

	//===========SET UP THE CAMERA===============
	glm::mat4 view_matrix = glm::lookAt(
		camera_position,					// eye
		camera_position + camera_front,	// centre
		camera_up);						// up

	//===========RENDER OBJECTS===============
	princess.Render(program, view_matrix, proj_matrix, camera_position);
	room.Render(program, view_matrix, proj_matrix, camera_position);
	dinosaur.Render(program, view_matrix, proj_matrix, camera_position);
	lights.Render(program, view_matrix, proj_matrix, camera_position);
	bed.Render(program, view_matrix, proj_matrix, camera_position);
	book.Render(program, view_matrix, proj_matrix, camera_position);
	banano.Render(program, view_matrix, proj_matrix, camera_position);
	mattress.Render(program, view_matrix, proj_matrix, camera_position);
	pillows.Render(program, view_matrix, proj_matrix, camera_position);
	night.Render(program, view_matrix, proj_matrix, camera_position);

	//===========FRAME BUFFER===============
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Disable rendering framebuffer to texture - aka render normally.
	glViewport(-windowWidth, -windowHeight, windowWidth * 2.0f, windowHeight * 2.0f);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(displayProgram);
	glUniform1f(glGetUniformLocation(displayProgram, "width"), windowWidth);
	glUniform1f(glGetUniformLocation(displayProgram, "height"), windowHeight);
	glBindVertexArray(displayVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse) {
		lastX = (float)mouseX; lastY = (float)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	  GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = (float)mouseX; lastY = (float)mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity; yoffset *= sensitivity;

	yaw += xoffset; pitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	camera_front = glm::normalize(front);
}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char*)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char*)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char*)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}
}