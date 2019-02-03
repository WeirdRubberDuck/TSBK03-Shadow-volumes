#include "Mesh.h"

// constructor
Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, GLuint ntris)
{
	this->vertices = vertices;
	this->indices = indices;
	this->ntris = ntris;

	// Adjacency disabled by default => empty
	this->indicesAdjacency = {};

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

// default constructor - only used for memory allocation
Mesh::Mesh()
{
	this->vertices = {};
	this->indices = {};
	this->indicesAdjacency = {};

	VAO = 0;
	VBO = 0;
	EBO = 0;
}

// render the mesh
void Mesh::render()
{
	glBindVertexArray(VAO);

	if (adjacency) {
		glDrawElements(GL_TRIANGLES_ADJACENCY, indicesAdjacency.size(), GL_UNSIGNED_INT, 0);
	} else {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}

// use adjacency information to render the triangle
void Mesh::useAdjacency()
{
	if ( adjacency ) return; // Adjency already enabled

	if ( indicesAdjacency.empty() ) genAdjacencyInfo();

	// Update buffers
	setupMesh();
}

void Mesh::disableAdjacency()
{
	adjacency = false;
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Index buffer, with or without adjacency information
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	if (adjacency) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesAdjacency.size() * sizeof(GLuint), &indicesAdjacency[0], GL_STATIC_DRAW);
	}
	else {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	}

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

// Create the data structures used for rendering triangles with adjacency info
void Mesh::genAdjacencyInfo()
{
	vector<GLuint> newIndices(6 * ntris);

	// Generate map that maps each vertex position to a unique index
	genUniqueIndexMap();

	for (int i = 0; i < ntris; i++)
	{
		GLuint firstIdx = 3 * i; // The first index of the triangle in the index array
		GLuint newFirstIdx = 6 * i;

		// Get unique index for each vertex in the triangle
		GLuint v[3];
		for (int j = 0; j < 3; j++)
		{
			GLuint idx = indices[firstIdx + j];
			v[j] = posIndexMap[vertices[idx].Position];
		}

		// Create new indexes to be used for adjacency mode
		for (int j = 0; j < 3; j++)
		{
			// Original vertice stays the same 
			newIndices[newFirstIdx + 2*j] = indices[firstIdx + j];

			// Find and add neighboring vertice to list
			GLuint n = findAdjacentVertexIdx(v[j], v[(j + 1) % 3], v[(j + 2) % 3]);
			assert(n != -1);
			newIndices[newFirstIdx + 2 * j + 1] = n;
		}
	}

	adjacency = true;
	indicesAdjacency = newIndices;
}

// Create map of unique vertice indexes, to use for adjacency information generation
// If a position vector is duplicated in the VB we only want the index of the first occurrence
void Mesh::genUniqueIndexMap()
{
	for (int i = 0; i < ntris; i++)
	{
		int firstIdx = 3 * i; // The first index of the triangle in the index array

		// For each vertex position, check if in the map.
		// If not, add it and the current indec. 
		for (int j = 0; j < 3; j++)
		{
			int idx = indices[firstIdx + j];
			glm::vec3 pos = vertices[idx].Position;

			if (posIndexMap.find(pos) == posIndexMap.end()) {
				posIndexMap[pos] = idx;
			}
		}
	}
}

// Find the neighboring vertex index of the edge given by startIdx and endIdx
// that differes from oppIdx. 
// OBS! Assumes same index for all vertices with same position
int Mesh::findAdjacentVertexIdx(GLuint startIdx, GLuint endIdx, GLuint oppIdx)
{
	//cout << "Edge given by: " << startIdx << " " << endIdx << endl;

	for (int i = 0; i < ntris; i++)
	{
		int firstIdx = 3 * i;

		GLuint faceIndices[3];
		for (int j = 0; j < 3; j++)
		{
			GLuint idx = indices[firstIdx + j];
			faceIndices[j] = posIndexMap[vertices[idx].Position];
		}

		// For each edge in the triangle, check its opposite vertice
		for (int edge = 0; edge < 3; edge++)
		{
			GLuint v0 = faceIndices[edge];
			GLuint v1 = faceIndices[(edge + 1) % 3];
			GLuint v2 = faceIndices[(edge + 2) % 3]; // Opposite index to side being tested

			if ((v0 == startIdx && v1 == endIdx) || (v1 == startIdx && v0 == endIdx))
			{
				// If last index is not the opposite of the edge we are comparing with
				// => we have found the adjacent vertex.
				if (v2 != oppIdx) return v2;
			}
		}
	}

	return 0;// -1; // no neighbor found
}