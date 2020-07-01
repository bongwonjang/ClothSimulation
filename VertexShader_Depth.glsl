#version 430

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() 
{
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}