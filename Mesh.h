/*
 *	Class for handling mesh information, like vetrices, indices and texture coordinates.
 *	Also includes a basic generation of adjacency information for each vertex, if that information is required.
 *
 *	Author: Emma Broman 
 */

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

const glm::vec3 ZERO(0.0f, 0.0f, 0.0f);

// Methods to compare glm::vec3 (needed for map)
struct CompVectors
{
	bool operator()(const glm::vec3& a, const glm::vec3& b)const {
		if (a.x < b.x) {
			return true;
		}
		else if (a.x == b.x) {
			if (a.y < b.y) {
				return true;
			}
			else if (a.y == b.y) {
				if (a.z < b.z) {
					return true;
				}
			}
		}
		return false;
	}
};

// Helper structure for vertex information
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
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	// constructor 
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLuint ntris);

	// default constructor - only used for memory allocation 
	Mesh();

	// render the mesh 
	void render();

	// use adjacency information to render the triangle
	void useAdjacency();

	// disable adjacency mode 
	void disableAdjacency();

private:
	//  Mesh Data  
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	GLuint VAO;
	GLuint ntris; // The number of triangles/faces

	// Render data  
	GLuint VBO, EBO;

	// Adjacency data
	bool adjacency = false;
	std::vector<GLuint> indicesAdjacency;
	std::map<glm::vec3, GLuint, CompVectors> posIndexMap; // Maps one unique index for every vertex position vector

	// initializes all the buffer objects/arrays 
	void setupMesh();

	// Create the data structures used for rendering triangles with adjacency info
	void genAdjacencyInfo();

	// Create map of unique vertice indexes, to use for adjacency information generation
	void genUniqueIndexMap();

	// find index of neighbor vertex to the edge (startIdx -> endIdx) 
	int findAdjacentVertexIdx(GLuint startIdx, GLuint endIdx, GLuint oppIdx);
};
#endif