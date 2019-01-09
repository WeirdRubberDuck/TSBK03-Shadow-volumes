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

using namespace std;

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

typedef map<glm::vec3, unsigned int, CompVectors> PosMap;

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
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	// constructor 
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned int ntris);

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
	vector<Vertex> vertices;
	vector<GLuint> indices;
	GLuint VAO;

	// Render data  
	GLuint VBO, EBO;

	int ntris; // The number of triangles/faces

	// Adjacency data
	bool adjacency = false;
	vector<GLuint> indices_adjacency;
	PosMap pos_index_map; // Maps one unique index for every vertex position vector

	// initializes all the buffer objects/arrays 
	void setupMesh();

	// Create the data structures used for rendering triangles with adjacency info
	void generateAdjacencyInfo();

	// find index of neighbor vertex to the edge (startIdx -> endIdx) 
	int findAdjacentVertexIdx(GLuint startIdx, GLuint endIdx, GLuint oppIdx);
};
#endif