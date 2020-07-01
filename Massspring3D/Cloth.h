#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Cloth
{
public:
	Cloth();
	~Cloth();

	/*
	For MassSpring
	*/
	void init(float stiffness);
	
	void update(glm::vec3 planeNormalVec, float planeD, glm::vec3 spherePos, float radius);
	void updateNormal();
	void control();
	glm::vec3 forceCal(int target, int source, float originalDis, float factor);
	void changeIsHold();

	float k;// Stiffness(N/m)
	const float m = 5.0;//Mass(kg)
	const float c = 100.0f;//Damping(kg/s)
	const float dt = 0.009;//Time(s) 
	float dx;//Initial distance(m)

	float clothSize = 10.0f;
	int numX;
	int numY;
	float radius = 0.03f;
	bool isHold = true;

	glm::vec3 * force;
	glm::vec3 * accel;
	glm::vec3 * velocity;

	/*
	For OpenGL
	*/
	void createClothMesh(float p_numX, float p_numY);
	bool generateVBOEBO();
	std::string readFile(const char *filePath);
	unsigned int createShader(const char * path, GLuint shaderMode);
	void loadShader(const char * vertexShaderPath, const char * fragmentShaderPath, int mode);
	bool loadTexture(const char * texturePath);

	unsigned int getVBOID();
	unsigned int getEBOID();
	int getShaderID();
	int getShadowShaderID();
	GLuint getTextureID();

	unsigned int VBOID;
	unsigned int EBOID;
	int shaderID;
	int shadowShaderID;
	GLuint textureID;
	unsigned char * textureData = NULL;

	int getSizeOfVertices();
	int getSizeOfNormals();
	int getSizeOfElements();
	int getSizeOfTextures();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textures;
	std::vector<unsigned int> elements;
};