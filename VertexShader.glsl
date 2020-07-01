#version 430

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;

out vec3 FragPos;
out vec3 outNormal;
out vec2 TexCoord;
out vec4 shadowTexCoord;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform mat4 MVP;

void main() {

	mat4 bias = mat4(0.5, 0.0, 0.0, 0.0,
					 0.0, 0.5, 0.0, 0.0,
					 0.0, 0.0, 0.5, 0.0,
					 0.5, 0.5, 0.5, 1.0);

	TexCoord = aTex;
	shadowTexCoord = bias * lightSpaceMatrix * model * vec4(aPos, 1.0);
	
	outNormal = mat3(transpose(inverse(model))) * aNormal;

	vec4 modelVec = model * vec4(aPos, 1.0);
	FragPos = vec3(modelVec.xyz);
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	gl_Position = MVP * vec4(aPos, 1.0);
}