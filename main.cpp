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
void drawShadowVolumes();
void drawLightSources();
void drawScene(Shader & objShader);
void createWindow(const unsigned int height, const unsigned int width, const char* name);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//----------------------Globals-------------------------------------------------
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool showShadowVolume = false;

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
Shader ambientShader, objShader, lampShader, geomShader, shadowVolumeShader;

// objects
Mesh object, object2, lamp;
Mesh ground, rightWall, leftWall, backWall;

// lighting
glm::vec3 lightPos(1.2f, 2.0f, 3.0f);

// colors
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 groundColor(0.6f, 0.6f, 0.6f);
glm::vec3 orange(1.0f, 0.5f, 0.2f);
glm::vec3 green(0.0f, 0.5f, 0.2f);

// matrices
float WALLSIZE = 4.0f;
glm::mat4 projection, view;
glm::mat4 objMat, obj2Mat, lampMat;

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

		// input
		// -----
		processInput(window);

		// render
		// ------	
		display(window);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
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
	//object = MeshCreator::createBox(0.3f, 0.3f, 0.2f);
	object = MeshCreator::readOBJ("meshes/torus_thingy.obj");
	object2 = MeshCreator::createBox(0.3f, 01.0f, 0.2f);
	lamp = MeshCreator::createSphere(0.1f, 10);

	ground = MeshCreator::createBox(WALLSIZE, 0.01f, WALLSIZE);
	rightWall = MeshCreator::createBox(0.01f, WALLSIZE, WALLSIZE);
	leftWall = MeshCreator::createBox(0.01f, WALLSIZE, WALLSIZE);
	backWall = MeshCreator::createBox(WALLSIZE, WALLSIZE, 0.01f);

	// Generate adjacency information for occluders
	object.useAdjacency();
	object2.useAdjacency();

	// Create static transformation matrices
	// -------------------------------------
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	obj2Mat = glm::translate(glm::mat4(), glm::vec3(0.5f, 0.0f, -2.0f));

	// Load and compile shaders
	// ------------------------
	ambientShader.create("shaders/ambientShader.vert", "shaders/ambientShader.frag");
	objShader.create("shaders/diffuseShader.vert", "shaders/diffuseShader.frag");
	lampShader.create("shaders/lamp.vert", "shaders/lamp.frag");
	geomShader.create("shaders/geomShader.vert", "shaders/geomShader.frag", "shaders/geomShader.geom");
	shadowVolumeShader.create("shaders/shadowVolume.vert", "shaders/shadowVolume.frag", "shaders/shadowVolume.geom");
}

// Display function - draws and renders!
//------------------------------------------------------------------------
void display(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// update matrices
	// ---------------
	view = camera.GetViewMatrix();

	// matrices used for object transformations in world space
	objMat = glm::mat4();
	float scale = 0.5f;
	objMat = glm::scale(objMat, glm::vec3(scale, scale, scale)); 
	objMat = glm::rotate(objMat, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	lampMat = glm::translate(glm::mat4(), lightPos);

	// Ambient pass: To make sure z-buffer contains data
	// ----------------------------------------
	drawScene(ambientShader);
	drawLightSources();

	// Create shadow volumes of objects and render into the stencil buffer 
	// ------------------------------------------------------------------
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);

	// need stencil test to be enabled but we want it to succeed always. 
	// Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0xFF);  // Set all stencil values to 0
		
	// Clamp depth values at infinity to max depth. Required for back cap of volume to be included
	// (Obs! requires depth test GL_EQUAL to include max value. GL_LESS is not enough)
	glEnable(GL_DEPTH_CLAMP);

	// do not render to depth or color buffer 
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// set stencil test according to zfail algorithm
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	drawShadowVolumes();

	// disable depth clamping
	glDisable(GL_DEPTH_CLAMP);

	// Render the scene again, using lightning and the stencil buffer as a mask for shadows
	// ------------------------------------------------------------------------------------

	// enable color buffer again
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// depth test only pass if depth value is the same as in ambient pass
	glDepthFunc(GL_EQUAL);

	// only draw if corresponding value in stencil buffer is zero
	glStencilFunc(GL_EQUAL, 0x0, 0xFF);

	// prevent update to the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	drawScene(objShader);
	drawLightSources();

	// Clean up: Reset some things needed for the ambient pass next frame
	// ------------------------------------------------------------------
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_STENCIL_TEST);
}

// render the shadow volumes of the occluders in the scene
// -------------------------------------------------------
void drawShadowVolumes()
{
	shadowVolumeShader.use();
	shadowVolumeShader.setMat4("projection", projection);
	shadowVolumeShader.setMat4("view", view);
	shadowVolumeShader.setVec3("lightPos", lightPos);

	shadowVolumeShader.setMat4("model", objMat);
	object.render();

	shadowVolumeShader.setMat4("model", obj2Mat);
	object2.render();
}

// render geometry for the light sources in the scene
// --------------------------------------------------
void drawLightSources()
{
	lampShader.use();
	lampShader.setMat4("projection", projection);
	lampShader.setMat4("view", view);
	lampShader.setVec3("lightColor", lightColor);
	lampShader.setMat4("model", lampMat);
	lamp.render();
}

// render the scene using the passed shader
// -----------------------------------------
void drawScene(Shader & objShader)
{
	objShader.use();
	objShader.setVec3("lightColor", lightColor);
	objShader.setVec3("lightPos", lightPos);
	objShader.setMat4("projection", projection);
	objShader.setMat4("view", view);

	// ground and walls
	objShader.setVec3("objectColor", groundColor);
	objShader.setMat4("model", glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)));
	ground.render();

	objShader.setMat4("model", glm::translate(glm::mat4(), glm::vec3(WALLSIZE, WALLSIZE - 1.0f, 0.0f)));
	rightWall.render();

	objShader.setMat4("model", glm::translate(glm::mat4(), glm::vec3(-WALLSIZE, WALLSIZE - 1.0f, 0.0f)));
	leftWall.render();

	objShader.setMat4("model", glm::translate(glm::mat4(), glm::vec3(0.0f, WALLSIZE - 1.0f,- WALLSIZE)));
	backWall.render();

	// objects
	objShader.setMat4("model", objMat);
	objShader.setVec3("objectColor", orange);
	object.render();

	objShader.setMat4("model", obj2Mat);
	objShader.setVec3("objectColor", green);
	object2.render();

	if (showShadowVolume) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		drawShadowVolumes();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
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
	glfwSetKeyCallback(window, key_callback);
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

// glfw: whenever a key is pressed, this callback is called
// -------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Show shadow volumes
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		showShadowVolume = !showShadowVolume;
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
