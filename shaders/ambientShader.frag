#version 330 core

uniform vec3 lightColor;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	vec3 shading = ambient * objectColor; 

    FragColor = vec4(shading, 1.0);
} 