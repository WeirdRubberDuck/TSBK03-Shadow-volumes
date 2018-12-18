#include <glad/glad.h>
#include <GLFW/glfw3.h>

// glm: linear algebra math, vectors and matrices in a glsl fashion
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "TriangleSoup.hpp"

#include <iostream>

void init();
void display(GLFWwindow* window);
void createWindow(const unsigned int height, const unsigned int width, const char* name);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//----------------------Globals-------------------------------------------------
GLFWwindow* window = nullptr;
Shader shaderProgram;
unsigned int VBO, VAO, EBO;

TriangleSoup object;

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
		display(window);
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// Initializion of shaders, etc
//--------------------------------------------------------------------------
void init()
{
	// GL inits
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST); // OBS! Depth test requires depth buffer...
	glDisable(GL_CULL_FACE);

	// Create geometry for rendering
	// -----------------------------
	object.createBox(0.5, 0.5, 0.0);

	// Load and compile shaders
	// ------------------------
	shaderProgram.create("shaders/shader.vert", "shaders/shader.frag");

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// create transformations

	glm::mat4 view;
	glm::mat4 projection;

	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	// object transformations
	glm::mat4 model;

	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	// activate shader program
	shaderProgram.use();

	// upload matrices to shader
	shaderProgram.setMat4("model", model);
	shaderProgram.setMat4("view", view);
	shaderProgram.setMat4("projection", projection);

	// draw objects
	object.render();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
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
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

