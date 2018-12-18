#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

// glm: linear algebra math, vectors and matrices in a glsl fashion
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	float vertices[] = {
		// positions          // texture coords
	 	 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	// Generate Vertex Array Object and buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // Copy vertex data to buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind, to be nice 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind VAO so other VAO calls won't accidentally modify this VAO (rarely happens)
	glBindVertexArray(0);

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
	//glEnable(GL_DEPTH_TEST); // OBS! Depth test requires depth buffer...
	glDisable(GL_CULL_FACE);

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
	glClear(GL_COLOR_BUFFER_BIT);

	// transformations
	glm::mat4 trans;
	//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	// Activate shader program
	shaderProgram.use();

	// Upload matrices to shader
	shaderProgram.setMat4("transform", trans);

	// draw triangle
	glBindVertexArray(VAO); // only have one VAO => no need to bind every time, but do it to keep things a bit more organized
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}


void createVBO(int location, int dimensions, const float *data) {

	GLuint bufferID;

	// Generate buffer, activate it and copy the data
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	// Tell OpenGL how the data is stored in our buffer
	// Attribute location (must match layout(location=#) statement in shader)
	// Number of dimensions (3 -> vec3 in the shader, 2-> vec2 in the shader),
	// type GL_FLOAT, not normalized, stride 0, start at element 0
	glVertexAttribPointer(location, dimensions, GL_FLOAT, GL_FALSE, 0, NULL);
	// Enable the attribute in the currently bound VAO
	glEnableVertexAttribArray(location);
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

