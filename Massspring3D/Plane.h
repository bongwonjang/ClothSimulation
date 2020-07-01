#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Plane
{
	/*
		I should have used inheritance.
	*/

public:
	Plane();
	~Plane();
	
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

	std::vector<glm::vec3> vertices{glm::vec3(10.0f, -4.0f, 10.0f), glm::vec3(10.0f, -4.0f, -10.0f),
							        glm::vec3(-10.0f, -4.0f, -10.0f), glm::vec3(-10.0f, -4.0f, 10.0f)};

	std::vector<glm::vec3> normals{glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 
								   glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)};

	std::vector<unsigned int> elements{0, 2, 1, 0, 3, 2};	

	/*
		These are values for geometrical plane information.

		Ax + By + Cz + D = 0
		But, as I use a simple plane, the normal vector and D are easily calced
	*/
	glm::vec3 getPlaneNormalVec();
	float getD();

	glm::vec3 normalVec = glm::vec3(0.0f, 1.0f, 0.0f);
	float D = 4.0f;


};