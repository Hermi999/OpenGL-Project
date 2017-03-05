#include <iostream>
#include <string>
#include <map>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Plane.h"
#include "Cube.h"
#include "Light.h"


// Function prototypes
GLFWwindow* initializeGame();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double offsetX, double offsetY);
void move_camera();
GLuint loadTexture(GLchar * path, GLboolean alpha);
Cube* createCube();
Plane* createPlane();
Light* createLight();

// Window dimensions
const GLuint WIDTH = 1600, HEIGHT = 900;

// Camera setup
Camera camera(glm::vec3(0.0f, 0.0f, 7.0f));
bool keys[1024];

// Balance velocity of camera
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Cursor setup
GLfloat lastX = 400, lastY = 300;
bool firstMouseInput = true;


// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Set up and initialize GLF, OpenGL, Key and Mouse Callbacks, the window, etc.
	GLFWwindow* window = initializeGame();

	// Prepare CUBES
	Cube* cube = createCube();
	cube->buildAndCompileShader("shaders/shader.vs", "shaders/shader.frag");
	cube->prepare(1);	// 1 ... vertices
	cube->positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));		// positions array holds 1 vec3 for each object which should be created
	cube->multiplyObject(glm::vec3(-150.0f, 10.0f, -150.0f), 1000, 10.0f);		// creates n objects @ a certain start position (2d)
	cube->multiplyObject(glm::vec3(-150.0f, 20.0f, -150.0f), 1000, 10.0f);
	cube->multiplyObject(glm::vec3(-150.0f, -10.0f, -150.0f), 1000, 10.0f);
	cube->multiplyObject(glm::vec3(-150.0f, -20.0f, -150.0f), 1000, 10.0f);
	cube->texture = loadTexture("textures/04pietrac4.png", false);

	// Prepare PLANES
	Plane* plane = createPlane();
	plane->buildAndCompileShader("shaders/plane.vs", "shaders/plane.frag");
	plane->prepare(0);	// 0 ... triangles
	plane->positions.push_back(glm::vec3(2.0f, 0.0f, 0.0f));
	plane->positions.push_back(glm::vec3(3.0f, 0.0f, -0.5f));
	GLfloat plane_color[] = { 0.1f, 0.5f, 0.1f, 0.3f };
	plane->setColor(plane_color);
	
	// Prepare Light source
	Light* light = createLight();
	light->buildAndCompileShader("shaders/light.vs", "shaders/light.frag");
	light->prepare(1);
	light->positions.push_back(glm::vec3(0.0f, 3.0f, 1.0f));
	GLfloat light_color[] = { 1.0f, 1.0f, 1.0f };
	light->setColor(light_color);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		move_camera();
		
		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// CAMERA
		glm::mat4 view = camera.GetViewMatrix();

		// PROJECTION
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);
		
		// Bind Textures, activate shader & draw object
		cube->bindTexture("cubeTexture");
		cube->activateShader(view, projection);
		cube->draw(camera, true);

		// activate plane shader, sort and draw planes
		plane->activateShader(view, projection);
		plane->sortAndDraw(camera, false);

		// draw light source
		light->activateShader(view, projection);
		light->draw(camera, false);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	delete cube;
	delete plane;
	delete light;
	
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

GLFWwindow* initializeGame() 
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CSE_Tuerk", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return window;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	
	if (firstMouseInput) {
		lastX = xpos;
		lastY = ypos;
		firstMouseInput = false;
	}

	GLfloat offsetX = xpos - lastX;
	GLfloat offsetY = lastY - ypos;	

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(offsetX, offsetY);

}

void scroll_callback(GLFWwindow* window, double offsetX, double offsetY) {
	camera.ProcessMouseScroll(offsetY);
}

void move_camera() {
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

GLuint loadTexture(GLchar * path, GLboolean alpha) {
	//Generate texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

	//Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);	// Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;

}

Cube* createCube()
{
	// 6 faces * 2 triangles * 3 vertices each
	GLfloat cubeVertices[] = {
		//Position			  // Texture Coordinates
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	// first triangle
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// second triangle
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,

		0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f,  0.5f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	Cube* cube = new Cube(cubeVertices, sizeof(cubeVertices));

	return cube;
}

Plane* createPlane()
{
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat planeVertices[] = {
		// Positions          // Colors           // Texture Coords
		0.5f,  0.5f, -1.0f,  // 1.0f, 0.0f, 0.0f,   //1.0f, 1.0f, // Top Right
		0.5f, -0.5f, -1.0f,  // 0.0f, 1.0f, 0.0f,   //1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,  // 0.0f, 0.0f, 1.0f,  // 0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,  // 1.0f, 1.0f, 0.0f,  // 0.0f, 1.0f  // Top Left 
	};
	GLuint planeIndices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	Plane* plane = new Plane(planeVertices,
										  sizeof(planeVertices),
										  planeIndices,
										  sizeof(planeIndices));

	return plane;
}

Light* createLight()
{
	// 6 faces * 2 triangles * 3 vertices each
	GLfloat lightVertices[] = {
		//Position		
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

	Light* light = new Light(lightVertices, sizeof(lightVertices));

	return light;
}