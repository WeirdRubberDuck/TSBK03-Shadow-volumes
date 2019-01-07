#include "MeshCreator.h"

/*
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void printError(const char *errtype, const char *errmsg)
{
	fprintf(stderr, "%s: %s\n", errtype, errmsg);
}

/* Create a simple triangle */
Mesh MeshCreator::createTriangle()
{
	vector<Vertex> vertices{
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)),
	};

	vector<unsigned int> indices{
		0,1,2
	};

	return Mesh(vertices, indices, 1);
}

/* Create a box with the dimension xsize * ysize * zsize */
Mesh MeshCreator::createBox(float xsize, float ysize, float zsize)
{
	const int nverts = 24;
	const int ntris = 12;

	const GLfloat vertices_data[8 * nverts] =
	{
		// TODO: Make sure texture coords are correct
		 xsize, -ysize,  zsize,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Vertex 0		0
		 xsize, -ysize,  zsize,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
		 xsize, -ysize,  zsize,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,

		 xsize, -ysize, -zsize,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // Vertex 1		3
		 xsize, -ysize, -zsize,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		 xsize, -ysize, -zsize,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,

		 xsize,  ysize, -zsize,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // Vertex 2		6
		 xsize,  ysize, -zsize,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
		 xsize,  ysize, -zsize,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,

		 xsize,  ysize,  zsize,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f, // Vertex 3		9
		 xsize,  ysize,  zsize,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
		 xsize,  ysize,  zsize,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,

		-xsize, -ysize,  zsize,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Vertex 4		12
		-xsize, -ysize,  zsize,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
		-xsize, -ysize,  zsize,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

		-xsize,  ysize,  zsize,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Vertex 5		15
		-xsize,  ysize,  zsize,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
		-xsize,  ysize,  zsize,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,

		-xsize,  ysize, -zsize,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // Vertex 6		18
		-xsize,  ysize, -zsize,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
		-xsize,  ysize, -zsize,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,

		-xsize, -ysize, -zsize,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // Vertex 7		21
		-xsize, -ysize, -zsize,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		-xsize, -ysize, -zsize,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f
	};

	vector<Vertex> vertices(nverts);

	for (int i = 0; i < nverts; i++)
	{
		float x = vertices_data[8 * i];
		float y = vertices_data[8 * i + 1];
		float z = vertices_data[8 * i + 2];
		glm::vec3 position{ x, y, z };

		float nx = vertices_data[8 * i + 3];
		float ny = vertices_data[8 * i + 4];
		float nz = vertices_data[8 * i + 5];
		glm::vec3 normal{ nx, ny, nz };

		float s = vertices_data[8 * i + 6];
		float t = vertices_data[8 * i + 7];
		glm::vec2 texCoords{ s, t };

		vertices[i] = Vertex{ position, normal, texCoords };
	}

	vector<unsigned int> indices{

		0,3,6,		// Face 1 - right
		0,6,9,

		2,11,14,	// Face 2 - front
		14,11,17,

		10,7,19,	// Face 3 - top
		10,19,16,

		1,22,4,		// Face 4 - bot
		1,13,22,

		12,15,21,	// Face 5 - left
		21,15,18,

		5,23,20,	// Face 6 - back
		5,20,8
	};

	return Mesh(vertices, indices, ntris);
}

/*
 * createSphere(float radius, int segments)
 *
 * Create a Mesh object with vertex and index arrays
 * to draw a textured sphere with normals.
 * Increasing the parameter 'segments' yields more triangles.
 * The vertex array is on interleaved format. For each vertex, there
 * are 8 floats: three for the vertex coordinates (x, y, z), three
 * for the normal vector (n_x, n_y, n_z) and finally two for texture
 * coordinates (s, t). The arrays are allocated by malloc() inside the
 * function and should be disposed of using free() when they are no longer
 * needed, e.g with the function soupDelete().
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2014.
 * This code is in the public domain.
 *
 * Modified by Emma Broman 2018 to work with the Mesh class used in this project. 
 */
Mesh MeshCreator::createSphere(float radius, int segments) {

	int i, j, base, i0;
	float x, y, z, R;
	double theta, phi;
	int vsegs, hsegs;
	int nverts, ntris;

	vsegs = segments;
	if (vsegs < 2) vsegs = 2;
	hsegs = vsegs * 2;
	nverts = 1 + (vsegs - 1) * (hsegs + 1) + 1; // top + middle + bottom
	ntris = hsegs + (vsegs - 2) * hsegs * 2 + hsegs; // top + middle + bottom

	vector<Vertex> vertices(nverts);
	vector<unsigned int> indices(ntris * 3);

	// The vertex array: 3D xyz, 3D normal, 2D st (8 floats per vertex)
	glm::vec3 position, normal;
	glm::vec2 texCoords;

	// First vertex: top pole (+z is "up" in object local coords)
	position  = { 0.0f, 0.0f, radius };
	normal    = { 0.0f, 0.0f, 1.0f };
	texCoords = { 0.5f, 1.0f };
	vertices[0] = Vertex{ position, normal, texCoords };

	// Last vertex: bottom pole
	position = { 0.0f, 0.0f, -radius };
	normal = { 0.0f, 0.0f, -1.0f };
	texCoords = { 0.5f, 0.0f };

	base = (nverts - 1);
	vertices[base] = Vertex{ position, normal, texCoords };

	// All other vertices:
	// vsegs-1 latitude rings of hsegs+1 vertices each
	// (duplicates at texture seam s=0 / s=1)
#ifndef M_PI
#define M_PI 3.1415926536
#endif // M_PI
	for (j = 0; j < vsegs - 1; j++) { // vsegs-1 latitude rings of vertices
		theta = (double)(j + 1) / vsegs * M_PI;
		z = cos(theta);
		R = sin(theta);
		for (i = 0; i <= hsegs; i++) { // hsegs+1 vertices in each ring (duplicate for texcoords)
			phi = (double)i / hsegs * 2.0*M_PI;
			x = R * cos(phi);
			y = R * sin(phi);

			position = { radius * x, radius * y, radius * z };
			normal = { x, y, z };
			texCoords = { 
				(float)i / hsegs, 
				1.0f - (float)(j + 1) / vsegs 
			};

			base = (1 + j * (hsegs + 1) + i);
			vertices[base] = Vertex{ position, normal, texCoords };
		}
	}

	// The index array: triplets of integers, one for each triangle
	// Top cap
	for (i = 0; i < hsegs; i++) {
		indices[3 * i] = 0;
		indices[3 * i + 1] = 1 + i;
		indices[3 * i + 2] = 2 + i;
	}
	// Middle part (possibly empty if vsegs=2)
	for (j = 0; j < vsegs - 2; j++) {
		for (i = 0; i < hsegs; i++) {
			base = 3 * (hsegs + 2 * (j*hsegs + i));
			i0 = 1 + j * (hsegs + 1) + i;
			indices[base] = i0;
			indices[base + 1] = i0 + hsegs + 1;
			indices[base + 2] = i0 + 1;
			indices[base + 3] = i0 + 1;
			indices[base + 4] = i0 + hsegs + 1;
			indices[base + 5] = i0 + hsegs + 2;
		}
	}
	// Bottom cap
	base = 3 * (hsegs + 2 * (vsegs - 2)*hsegs);
	for (i = 0; i < hsegs; i++) {
		indices[base + 3 * i] = nverts - 1;
		indices[base + 3 * i + 1] = nverts - 2 - i;
		indices[base + 3 * i + 2] = nverts - 3 - i;
	}

	return Mesh(vertices, indices, ntris);
}

/*
 * readObj(const char* filename)
 *
 * Load geometry data from an OBJ file.
 * The vertex array is on interleaved format. For each vertex, there
 * are 8 floats: three for the vertex coordinates (x, y, z), three
 * for the normal vector (n_x, n_y, n_z) and finally two for texture
 * coordinates (s, t). The returned arrays are allocated by malloc()
 * inside the function and should be disposed of using free() when
 * they are no longer needed, e.g. by calling soupDelete().
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2014.
 * This code is in the public domain.
 *
 * Modified by Emma Broman 2018 to fit with the Mesh class used in this project. 
 */
Mesh MeshCreator::readOBJ(const char* filename) {

	FILE *objfile;

	int numverts = 0;
	int numnormals = 0;
	int numtexcoords = 0;
	int numfaces = 0;
	int i_v = 0;
	int i_n = 0;
	int i_t = 0;
	int i_f = 0;
	float *verts, *normals, *texcoords;

	char line[256];
	char tag[3];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int numargs, readerror, currentv;

	readerror = 0;

	objfile = fopen(filename, "r");

	if (!objfile) {
		printError("File not found", filename);
		readerror = 1;
	}

	// Scan through the file to count the number of data elements
	while (fgets(line, 256, objfile)) {
		sscanf(line, "%2s ", tag);
		if (!strcmp(tag, "v")) numverts++;
		else if (!strcmp(tag, "vn")) numnormals++;
		else if (!strcmp(tag, "vt")) numtexcoords++;
		else if (!strcmp(tag, "f")) numfaces++;
		//else printf("Ignoring line starting with \"%s\"\n", tag);
	}

	printf("loadObj(\"%s\"): found %d vertices, %d normals, %d texcoords, %d faces.\n",
		filename, numverts, numnormals, numtexcoords, numfaces);

	verts = new float[3 * numverts];
	normals = new float[3 * numnormals];
	texcoords = new float[2 * numtexcoords];

	int nverts = 3 * numfaces;
	int ntris = numfaces;

	vector<Vertex> vertices(nverts);
	vector<unsigned int> indices(ntris * 3);

	rewind(objfile); // Start from the top again to read data

	while (fgets(line, 256, objfile)) {
		tag[0] = '\0';
		sscanf(line, "%2s ", tag);
		if (!strcmp(tag, "v")) {
			//			printf("Reading vertex %d\n", i_v+1);
			numargs = sscanf(line, "v %f %f %f",
				&verts[3 * i_v], &verts[3 * i_v + 1], &verts[3 * i_v + 2]);
			if (numargs != 3) {
				printf("Malformed vertex data found at vertex %d.\n", i_v + 1);
				printf("Aborting.\n");
				readerror = 1;
				break;
			}
			//			printf("Read vertex coord %d: %8.2f %8.2f %8.2f\n",
			//			i_v, verts[3*i_v], verts[3*i_v+1], verts[3*i_v+2]);
			i_v++;
		}
		else if (!strcmp(tag, "vn")) {
			//			printf("Reading normal %d\n", i_n+1);
			numargs = sscanf(line, "vn %f %f %f",
				&normals[3 * i_n], &normals[3 * i_n + 1], &normals[3 * i_n + 2]);
			if (numargs != 3) {
				printf("Malformed normal data found at normal %d.\n", i_n + 1);
				printf("Aborting.\n");
				readerror = 1;
				break;
			}
			i_n++;
		}
		else if (!strcmp(tag, "vt")) {
			//          printf("Reading texcoord %d\n", i_t+1);
			numargs = sscanf(line, "vt %f %f",
				&texcoords[2 * i_t], &texcoords[2 * i_t + 1]);
			if (numargs != 2) {
				printf("Malformed texcoord data found at texcoord %d.\n", i_t + 1);
				printf("Aborting.\n");
				readerror = 1;
				break;
			}
			i_t++;
		}
		else if (!strcmp(tag, "f")) {
			//			printf("Reading face %d\n", i_f+1);
			numargs = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			if (numargs != 9) {
				printf("Malformed face data found at face %d.\n", i_f + 1);
				printf("Aborting.\n");
				readerror = 1;
				break;
			}
			//			printf("Read vertex data %d/%d/%d %d/%d/%d %d/%d/%d\n",
			//			v1, t1, n1, v2, t2, n2, v3, t3, n3);
			v1--; v2--; v3--; n1--; n2--; n3--; t1--; t2--; t3--;
			currentv = 3 * i_f;

			float x, y, z, nx, ny, nz, s, t;

			x = verts[3 * v1];
			y = verts[3 * v1 + 1];
			z = verts[3 * v1 + 2];
			nx = normals[3 * n1];
			ny = normals[3 * n1 + 1];
			nz = normals[3 * n1 + 2];
			s = texcoords[2 * t1];
			t = texcoords[2 * t1 + 1];

			vertices[currentv] = Vertex{ glm::vec3(x, y, z), glm::vec3(nx, ny, nz), glm::vec2(s, t) };

			x = verts[3 * v2];
			y = verts[3 * v2 + 1];
			z = verts[3 * v2 + 2];
			nx = normals[3 * n2];
			ny = normals[3 * n2 + 1];
			nz = normals[3 * n2 + 2];
			s = texcoords[2 * t2];
			t = texcoords[2 * t2 + 1];

			vertices[currentv+1] = Vertex{ glm::vec3(x, y, z), glm::vec3(nx, ny, nz), glm::vec2(s, t) };

			x = verts[3 * v3];
			y = verts[3 * v3 + 1];
			z = verts[3 * v3 + 2];
			nx = normals[3 * n3];
			ny = normals[3 * n3 + 1];
			nz = normals[3 * n3 + 2];
			s = texcoords[2 * t3];
			t = texcoords[2 * t3 + 1];

			vertices[currentv+2] = Vertex{ glm::vec3(x, y, z), glm::vec3(nx, ny, nz), glm::vec2(s, t) };

			// The index array:
			indices[3 * i_f] = 3 * i_f;
			indices[3 * i_f + 1] = 3 * i_f + 1;
			indices[3 * i_f + 2] = 3 * i_f + 2;
			i_f++;
		};
	}

	// Clean up the temporary arrays we created
	delete[] verts; verts = NULL;
	delete[] normals; normals = NULL;
	delete[] texcoords; texcoords = NULL;
	fclose(objfile);

	if (readerror) { // Delete corrupt data and bail out if a read error occured
		printError("Mesh read error", "No mesh data generated");
		return Mesh();
	}

	printf("loadObj(\"%s\"): found %d vertices, %d normals, %d texcoords, %d faces.\n",
		filename, numverts, numnormals, numtexcoords, numfaces);

	return Mesh(vertices, indices, ntris);
}
