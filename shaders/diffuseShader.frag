#version 330 core

in vec3 normal;
in vec3 lightDir;

uniform vec3 lightColor;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 shading = (ambient + diffuse) * objectColor; 

    FragColor = vec4(shading, 1.0);
} 