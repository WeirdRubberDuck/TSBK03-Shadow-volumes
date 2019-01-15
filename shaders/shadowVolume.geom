#version 330 core
layout (triangles_adjacency) in; // 6 vertices
layout (triangle_strip, max_vertices = 18) out;

uniform vec3 lightPos;

// TODO: multiply these on CPU instead?
uniform mat4 projection;
uniform mat4 view;

float EPSILON = 0.0001;

mat4 PVM = projection * view;

void ExtrudeEdge(vec3 startVertex, vec3 endVertex)
{
	// Start vertex. Original and projected to infinity
    vec3 lightDir = normalize(startVertex - lightPos);
	gl_Position = PVM* vec4((startVertex + lightDir * EPSILON), 1.0);
	EmitVertex();
	gl_Position = PVM  * vec4(lightDir, 0.0);
    EmitVertex();

	// End vertex. Original and projected to infinity
	lightDir = normalize(endVertex - lightPos);
    gl_Position = PVM * vec4((endVertex + lightDir * EPSILON), 1.0);
    EmitVertex();
	gl_Position = PVM * vec4(lightDir , 0.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 lightDirs[3];  // Direction towards light in each vertex of a triangle
	vec3 normals[3];	// Normal in each vertex of a triangle

	// Vertices as vec3
	vec3 vertPos[6];
	for(int i=0; i < 6; i++) {
		vertPos[i] =  gl_in[i].gl_Position.xyz;
	}

	// Edges (first three belong to main triangle)
	vec3 edge[6];
	edge[0] = vertPos[2] - vertPos[0]; 
    edge[1] = vertPos[4] - vertPos[2];
    edge[2] = vertPos[0] - vertPos[4];
	edge[3] = vertPos[1] - vertPos[0];
    edge[4] = vertPos[3] - vertPos[2];
    edge[5] = vertPos[5] - vertPos[4];

	vec3 mainNormal = cross(edge[0],edge[1]); // normal of main triangle
	vec3 lightDir = normalize(vertPos[0] - lightPos);

	// if main triangle not facing light, ignore (do nothing)
	if (dot(mainNormal, lightDir) > 0) return;

	// If it does, check its edges and extrude if needed
	for (int i = 0; i < 3; i++) {
		// Compute indices of start, end and neighbor vertice
		int start = i*2;
		int neighbor = i*2+1;
		int end = (i*2+2) % 6; 
		
		// compute normal of neighbor triangle (same in each vertex)
		vec3 normal = cross(edge[i+3], edge[i]); 

		// compute light direction in each vertex pos
		lightDirs[0] = normalize(vertPos[start] - lightPos);
		lightDirs[1] = normalize(vertPos[neighbor] - lightPos);
		lightDirs[2] = normalize(vertPos[end] - lightPos);

		// Extrude if possible silhuette (neighbor triangle does not face light)	// TODO: implement code for no neighbor
		if(dot(normal, lightDirs[0]) > 0 ) {
			ExtrudeEdge(vertPos[start], vertPos[end]);
		}
	} 

	// Render front cap 
	for (int i = 0; i < 3; i++) {
		lightDir = normalize(vertPos[2*i] - lightPos);
		gl_Position = PVM* vec4((vertPos[2*i] + lightDir * EPSILON), 1.0);
		EmitVertex();
	}
	EndPrimitive();

	// Render back cap 
	for (int i = 0; i < 3; i++) {
		lightDir = vertPos[2*i] - lightPos;
		gl_Position = PVM* vec4(lightDir, 0.0);
		EmitVertex();
	}
	EndPrimitive();
} 