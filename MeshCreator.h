#ifndef MESHCREATOR_H
#define MESHCREATOR_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

class MeshCreator {

public:

	/* Create a simple triangle */
	static Mesh createTriangle();

	/* Create a box with the dimension xsize * ysize * zsize */
	static Mesh createBox(float xsize, float ysize, float zsize);

	/* Create a sphere (approximated by polygon segments) */
	static Mesh createSphere(float radius, int segments);

	/* Load geometry from an OBJ file */
	static Mesh readOBJ(const char* filename);

};
#endif