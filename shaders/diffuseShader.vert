#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 pos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

	// world space position and normals
	mat3 normalMatrix = mat3(transpose(inverse(model))); // to handle non-uniform scaling correctly
	vec3 transNormal = normalMatrix * aNormal; 
	vec3 transPos = mat3(model) * aPos;

	// to pass to fragment shader
	normal = normalize(transNormal);
	pos = transPos;
}