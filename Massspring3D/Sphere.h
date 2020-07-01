#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Sphere
{
public:
	Sphere();
	~Sphere();

	bool loadOBJ(const char * objPath);
	bool generateVBOEBO();
	std::string readFile(const char *filePath);
	unsigned int createShader(const char * path, GLuint shaderMode);
	void loadShader(const char * vertexShaderPath, const char * fragmentShaderPath, int mode);

	unsigned int getVBOID();
	unsigned int getEBOID();
	int getShaderID();
	int getShadowShaderID();

	unsigned int VBOID;
	unsigned int EBOID;
	int shaderID;
	int shadowShaderID;

	int getSizeOfVertices();
	int getSizeOfNormals();
	int getSizeOfElements();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> elements;

	/*
		These are values for the geometric information of this sphere.
		(x-a)^2 + (y-b)^2 + (z-c)^2 = r^2
	*/
	float getRadius();
	glm::vec3 getPos();
	void setPos(glm::vec3 newPos);
	float radius = 1.5f;
	glm::vec3 centerPos;
};