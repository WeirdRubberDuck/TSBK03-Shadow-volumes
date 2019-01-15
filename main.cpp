#include <glad/glad.h>
#include <GLFW/glfw3.h>

// glm: linear algebra math, vectors and matrices in a glsl fashion
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "MeshCreator.h"

#include <iostream>

void init();
void display(GLFWwindow* window);
void drawShadowVolume();
void drawScene(Shader & objShader, Shader & lampShader);

void createWindow(const unsigned int height, const unsigned int width, const char* name);
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//----------------------Globals-------------------------------------------------
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLFWwindow* window = nullptr;

// camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Mouse position and click information
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
int lastLeft = GL_FALSE; // Keeps track if mouse button was clicked last frame or not

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// shaders
Shader ambientShader, depthShader, objShader, lampShader, geomShader, shadowVolumeShader;

// objects
Mesh object, lamp, ground;

// lighting
glm::vec3 lightPos(1.2f, 2.0f, 3.0f);

// colors
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 objectColor(1.0f, 0.5f, 0.2f);
glm::vec3 groundColor(0.8f, 0.8f, 0.8f);

// matrices
glm::mat4 projection, view;
glm::mat4 objMatrix, groundMatrix, lampMatrix;

//----------------------Main----------------------------------------------------
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	createWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window");

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Initialization 
	// --------------
	init();

	// Wireframe mode: uncomment to apply.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Show some useful information on the GL context
	std::cout << "GL vendor:       " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL renderer:     " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL version:      " << glGetString(GL_VERSION) << std::endl;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// update time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		display(window);
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// Initializion of shaders, etc
//--------------------------------------------------------------------------
void init()
{
	// Get initialized mouse info
	glfwGetCursorPos(window, &lastX, &lastY);

	// GL inits
	glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST); // OBS! Depth test requires depth buffer...
	glDisable(GL_CULL_FACE);

	// Create geometry for rendering
	// -----------------------------
	object = MeshCreator::createBox(0.5f, 0.5f, 0.2f);
	//object = MeshCreator::readOBJ("meshes/teapot_coarse.obj");
	//object = MeshCreator::createSphere(20.0f, 10);
	object.useAdjacency();
	lamp = MeshCreator::createSphere(0.1f, 10);
	ground = MeshCreator::createBox(5.0f, 0.01f, 5.0f);

	// Create static transformation matrices
	// -------------------------------------
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix(); // OBS! Has to be updated later
	groundMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f));
	lampMatrix = glm::translate(glm::mat4(), lightPos);

	// Load and compile shaders
	// ------------------------
	ambientShader.create("shaders/ambientShader.vert", "shaders/ambientShader.frag");
	depthShader.create("shaders/depthShader.vert", "shaders/depthShader.frag");
	objShader.create("shaders/diffuseShader.vert", "shaders/diffuseShader.frag");
	lampShader.create("shaders/lamp.vert", "shaders/lamp.frag");
	geomShader.create("shaders/geomShader.vert", "shaders/geomShader.frag", "shaders/geomShader.geom");
	shadowVolumeShader.create("shaders/shadowVolume.vert", "shaders/shadowVolume.frag", "shaders/shadowVolume.geom");
}

// Display function - draws and renders!
//------------------------------------------------------------------------
void display(GLFWwindow* window)
{
	// input
	// -----
	processInput(window);

	// render
	// ------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// update matrices
	// ---------------
	view = camera.GetViewMatrix();

	// Matrices used for object transformations in world space
	objMatrix = glm::mat4();
	//float scale = 0.03f;
	//objTransMatrix = glm::scale(objTransMatrix, glm::vec3(scale, scale, scale)); // Teapot is superhuge!!
	objMatrix = glm::rotate(objMatrix, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	// Ambient pass: To make sure z-buffer contains data
	// ----------------------------------------
	drawScene(ambientShader, ambientShader);

	// Create shadow volumes of objects and render into the stencil buffer 
	// ------------------------------------------------------------------

	glEnable(GL_STENCIL_TEST);

	// need stencil test to be enabled but we want it to succeed always. 
	// Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0xFF);  // Set all stencil values to 0

	// TODO: If camera is inside shadow volume, init with 1 instead

	// Depth testing and face culling required
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Do not render to depth or color buffer 
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// front triangles: increment if depth test fails (zfail)
	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);  
	drawShadowVolume();

	// back triangles: decrement if depth test fails (caps)
	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);		
	drawShadowVolume();


	// Render the scene again, using lightning and the stencil buffer as a mask for shadows
	// ------------------------------------------------------------------------------------

	// Enable color buffer again
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Depth test only pass if depth value is the same as in ambient pass
	glDepthFunc(GL_EQUAL);

	// Disable cull face again, to be able to see inside of rendered objects
	glDisable(GL_CULL_FACE);

	// only draw if corresponding value in stencil buffer is zero
	glStencilFunc(GL_EQUAL, 0x0, 0xFF);

	// prevent update to the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	drawScene(objShader, lampShader);

	// Clean up: Reset some things needed for the ambient pass next frame
	// ------------------------------------------------------------------
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_STENCIL_TEST);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void drawShadowVolume()
{
	shadowVolumeShader.use();
	shadowVolumeShader.setVec3("lightPos", lightPos);
	shadowVolumeShader.setMat4("projection", projection);
	shadowVolumeShader.setMat4("view", view);
	shadowVolumeShader.setMat4("model", objMatrix);
	object.render();
}

void drawScene(Shader & objShader, Shader & lampShader)
{
	objShader.use();
	objShader.setVec3("lightColor", lightColor);
	objShader.setVec3("lightPos", lightPos);
	objShader.setMat4("projection", projection);
	objShader.setMat4("view", view);

	// ground
	objShader.setMat4("model", groundMatrix);
	objShader.setVec3("objectColor", groundColor);
	ground.render();

	// objects
	objShader.setMat4("model", objMatrix);
	objShader.setVec3("objectColor", objectColor);
	object.render();

	// lamp
	lampShader.use();
	lampShader.setMat4("projection", projection);
	lampShader.setMat4("view", view);
	lampShader.setVec3("lightColor", lightColor);
	lampShader.setMat4("model", lampMatrix);
	lamp.render();
}

// glfw window creation
// --------------------
void createWindow(const unsigned int height, const unsigned int width, const char* name)
{
	window = glfwCreateWindow(height, width, name, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// Light position
	float deltaStep = 0.001f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPos += glm::vec3(0.0f, deltaStep * 1.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos += glm::vec3(0.0f, deltaStep * (-1.0f), 0.0f);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos += glm::vec3( deltaStep * (-1.0f), 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos += glm::vec3( deltaStep * 1.0f, 0.0f, 0.0f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Check if left mouse button is pressed
	int currentLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	// Rotate camera based on mouse input
	if (currentLeft && lastLeft) { // If a left button drag is in progress
		float moveX = xpos - lastX;
		float moveY = lastY - ypos;	// Inverted
		camera.ProcessMouseMovement(moveX, moveY);
	}

	// Update cursor information
	lastLeft = currentLeft;
	lastX = xpos;
	lastY = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
