#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

const glm::vec3 ZERO(0.0f, 0.0f, 0.0f);

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;

	Vertex(glm::vec3 p = ZERO, glm::vec3 n = ZERO, glm::vec2 tc = ZERO)
		: Position(p), Normal(n), TexCoords(tc) {}
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	unsigned int VAO;

	/* constructor */
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

	/* default constructor - only used for memory allocation */
	Mesh();

	/* render the mesh */
	void render();

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/* initializes all the buffer objects/arrays */
	void setupMesh();
};
#endif