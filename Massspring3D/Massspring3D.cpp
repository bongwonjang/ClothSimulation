#include "pch.h"
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glew32.lib")

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "Plane.h"
#include "Cloth.h"
#include "Sphere.h"

/*
The window size:
	Width = 1024
	Height = 1024
*/
float WIDTH = 512;
float HEIGHT = 512;

/*
	Vertex Array Object for Cloth, Plane, Sphere
*/
unsigned int VAOID;

/*
	Depth Map Frame Buffer
*/
unsigned int FBOID;
unsigned int TEXID;
unsigned int RBOID;

/*
	Objects for the Scene graph:
		Camera doesn't have default setting.
		Plane, Cloth, Sphere have default setting.
*/
Camera * camera = NULL;
Plane plane;
Cloth cloth;
Sphere sphere;

//Light Posision. This is important.
glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 10.0f);
glm::vec3 lightUp = glm::vec3(0.0f, -1.0f, -1.0f);
/*
	Initialize function.
	Initialize frame buffer and initialize scene graph's nodes.
*/
void initializeFrameBuffer();
void initalizeNode();

/*
	Callback function.

	mouseClick and mouseMotion	: mouse zoom and rotate event
	timeFunc					: update simulation
	keyboardFunc				: press WASD and Z and space bar
	display						: render scene.
*/
void mouseClick(int mouseEvt, int state, int x, int y);
	bool leftMouseClicked = false;
	bool middleMouseClicked = false;
	int mousePos[2] = { 0, 0 }; // point x and point y
void mouseMotion(int x, int y);
void timeFunc(int value);
void keyboardFunc(unsigned char key, int x, int y);
void display();

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA | GLUT_STENCIL);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(1, 1);
	glutCreateWindow("2015410004 ÀåºÀ¿ø");
	
	glewInit();

	initalizeNode();

	glutDisplayFunc(display);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboardFunc);
	glutTimerFunc(5, timeFunc, 1);

	glutMainLoop();
	
	glDeleteVertexArrays(1, &VAOID);
	camera = NULL;
	
	return 0;
}
void keyboardFunc(unsigned char key, int x, int y)
{
	/*
		This function is for moving cloth or sphere.
			' '				: give force to the cloth
			'z', 'Z'		: hold or unhold the cloth
			'w', 'W'		: move sphere forward 
			'a', 'A'		: move sphere left
			's', 'S'		: move sphere backward
			'd', 'D'		: move sphere right
 	*/

	glm::vec3 curSpherePos = sphere.getPos(); //Get current sphere position
	
	switch (key) 
	{
	case ' ':
		cloth.control();
		break;
	case 'z':
	case 'Z':
		cloth.changeIsHold();
		break;
	case 'w':
	case 'W':
		sphere.setPos(curSpherePos + glm::vec3(0.0f, 0.0f, 5.0f) * 0.01f);
		break;
	case 's':
	case 'S':
		sphere.setPos(curSpherePos + glm::vec3(0.0f, 0.0f, -5.0f)* 0.01f);
		break;
	case 'a':
	case 'A':
		sphere.setPos(curSpherePos + glm::vec3(-5.0f, 0.0f, 0.0f)* 0.01f);
		break;
	case 'd':
	case 'D':
		sphere.setPos(curSpherePos + glm::vec3(5.0f, 0.0f, 0.0f)* 0.01f);
		break;
	}
	glutPostRedisplay();
}
void timeFunc(int value)
{
	/*
		Update cloth simulation
		Simply, just give one plane info and one sphere info

		plane	: Ax + By + Cz + D = 0
				-> (A, B, C)	: normal vector
				-> (D)			: parameter
		
		sphere	: (x-a)^2 + (y-b)^2 + (z-c)^2  = r^2
				-> (a, b, c)	: sphere center
				-> (r)			: radius
	*/
	cloth.update(plane.getPlaneNormalVec(), plane.getD(), sphere.centerPos, sphere.getRadius());
	
	glutPostRedisplay();
	glutTimerFunc(5, timeFunc, 1);
}
void mouseClick(int mouseEvt, int state, int x, int y)
{
	mousePos[0] = x;
	mousePos[1] = y;

	if (mouseEvt == GLUT_MIDDLE_BUTTON)
	{
		middleMouseClicked = (GLUT_DOWN == state);
		leftMouseClicked = false;
	}
	else if (mouseEvt == GLUT_LEFT_BUTTON)
	{
		leftMouseClicked = (GLUT_DOWN == state);
		middleMouseClicked = false;
	}

}
void mouseMotion(int x, int y)
{
	/*
		Middle Mouse : Rotate Camera Based on Sphere Coordinate
		Left Mouse : Zoom in / out with Field of View
	*/
	if (middleMouseClicked)
	{
		camera->subYAW((x - mousePos[0]) * 0.0001f);
		camera->subPITCH((y - mousePos[1]) * 0.0001f);

		camera->updateCameraPos();
	}
	else if (leftMouseClicked)
	{
		if (y - mousePos[1] < 0)
			camera->addFOV(0.4f);
		else
			camera->subFOV(0.4f);
	}

	glutPostRedisplay();
}
void initializeFrameBuffer()
{
	//Generate and Bind Frame Buffer
	glGenFramebuffers(1, &FBOID);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOID);

	//Generate and Bind Texture for Frame
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &TEXID);
	glBindTexture(GL_TEXTURE_2D, TEXID);

	//Initialize Texture MipMap. This is really important part.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//Attach the texture to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TEXID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);

	//Unbind FBO and Texture for frame
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void initalizeNode()
{
	glEnable(GL_DEPTH_TEST); // You should enable this in order to use depth
	initializeFrameBuffer();

	camera = new Camera(0, 0, 35); //Initialize Camera Node

	glGenVertexArrays(1, &VAOID); //Initialzie VAO
	glBindVertexArray(VAOID);
	
	/*
		Create cloth object

		This object is created in this program.
		All texture coordinate and normal vectors are calculated.
	*/
	cloth.createClothMesh(32, 32);
	if (!cloth.generateVBOEBO())
		std::cout << "Cloth Error in VBO and EBO" << std::endl;
	else
		std::cout << "Cloth Load safely VBO and EBO" << std::endl;
	cloth.loadShader("../VertexShader.glsl", "../FragmentShader.glsl", 0);
	cloth.loadShader("../VertexShader_Depth.glsl", "../FragmentShader_Depth.glsl", 1);

	std::cout << cloth.getShaderID() << " !!! " << cloth.getShadowShaderID() << std::endl;

	cloth.loadTexture("..\\texture\\check.jpg");

	cloth.init(30000.0f); //Stiffness. 30000.0f is enough

	/*
		plane

		This object is created in this program.
		All texture coordinate and normal vectors are calculated.
	*/
	if (!plane.generateVBOEBO())
		std::cout << "Plane Error in VBO and EBO" << std::endl;
	else
		std::cout << "Plane Load safely VBO and EBO" << std::endl;

	plane.loadShader("../VertexShaderPlane.glsl", "../FragmentShaderPlane.glsl", 0);
	plane.loadShader("../VertexShader_Depth.glsl", "../FragmentShader_Depth.glsl", 1);//Same
	
	std::cout << plane.getShaderID() << " ??? " << plane.getShadowShaderID() << std::endl;

	/*
		Sphere

		This object is loaded from sphere2.obj (sphere.obj is ico spehere).
		We don't use texture on this object
	*/
	sphere.loadOBJ("../obj/sphere2.obj");
	if (!sphere.generateVBOEBO())
		std::cout << "Sphere Error in VBO and EBO" << std::endl;
	else
		std::cout << "Sphere Load safely VBO and EBO" << std::endl;

	sphere.loadShader("../VertexShaderSphere.glsl", "../FragmentShaderSphere.glsl", 0);
	sphere.loadShader("../VertexShader_Depth.glsl", "../FragmentShader_Depth.glsl", 1);

	std::cout << sphere.getShaderID() << " ^^^ " << sphere.getShadowShaderID() << std::endl;

	//Finishing Data Transfer, then Unbind VAO
	glBindVertexArray(0);
}

/*
	I just separated each node's rendering sequence.
	This makes unnecessary codes but help us to understand how each render works. 
*/
void drawFBOSphere()
{
	glBindBuffer(GL_ARRAY_BUFFER, sphere.getVBOID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.getEBOID());

	glm::mat4 matrix(1.0f);
	matrix = glm::scale(matrix, glm::vec3(sphere.getRadius()));
	matrix[0][3] = sphere.getPos().x;
	matrix[1][3] = sphere.getPos().y;
	matrix[2][3] = sphere.getPos().z;

	//Get Camera State for MVP transfer etc
	glm::vec3 cameraPos = camera->getCameraPos();
	glm::mat4 mvp = camera->getMVP(WIDTH, HEIGHT, matrix);
	glm::mat4 modelView = glm::transpose(matrix);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.01f, far_plane = 50.0f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), lightUp);
	lightSpaceMatrix = lightProjection * lightView;

	GLint shadowShaderID = sphere.getShadowShaderID();
	glUseProgram(shadowShaderID);

	/*
		In Vertex Shader

		layout 0 = aPos
		layout 1 = aColor <- not used
		layout 2 = aTex <- not used 
		layout 3 = aNormal
	*/
	int aPos = glGetAttribLocation(shadowShaderID, "aPos");
	int aNormal = glGetAttribLocation(shadowShaderID, "aNormal");

	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	int MVP = glGetUniformLocation(shadowShaderID, "MVP");
	int model = glGetUniformLocation(shadowShaderID, "model");
	int LSM = glGetUniformLocation(shadowShaderID, "lightSpaceMatrix");
	int viewPos = glGetUniformLocation(shadowShaderID, "viewPos");

	glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &modelView[0][0]);
	glUniformMatrix4fv(LSM, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

	glDrawElements(GL_TRIANGLES, sphere.getSizeOfElements(), GL_UNSIGNED_INT, 0);
	
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void drawFBOPlane()
{
	glBindBuffer(GL_ARRAY_BUFFER, plane.getVBOID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.getEBOID());

	glm::mat4 matrix(1.0f);

	//Get Camera State for MVP transfer etc
	glm::vec3 cameraPos = camera->getCameraPos();
	glm::mat4 mvp = camera->getMVP(WIDTH, HEIGHT, matrix);
	glm::mat4 modelView = glm::transpose(matrix);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.01f, far_plane = 50.0f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), lightUp);
	lightSpaceMatrix = lightProjection * lightView;

	GLint shadowShaderID = plane.getShadowShaderID();
	glUseProgram(shadowShaderID);

	/*
		In Vertex Shader

		layout 0 = aPos
		layout 1 = aColor <- not used
		layout 2 = aTex <- not used
		layout 3 = aNormal
	*/
	int aPos = glGetAttribLocation(shadowShaderID, "aPos");
	int aNormal = glGetAttribLocation(shadowShaderID, "aNormal");

	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(plane.getSizeOfVertices() * sizeof(glm::vec3)));

	int MVP = glGetUniformLocation(shadowShaderID, "MVP");
	int model = glGetUniformLocation(shadowShaderID, "model");
	int LSM = glGetUniformLocation(shadowShaderID, "lightSpaceMatrix");
	int viewPos = glGetUniformLocation(shadowShaderID, "viewPos");

	glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &modelView[0][0]);
	glUniformMatrix4fv(LSM, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

	glDrawElements(GL_TRIANGLES, plane.getSizeOfElements(), GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void drawFBOCloth()
{
	glBindBuffer(GL_ARRAY_BUFFER, cloth.getVBOID());
	glBufferSubData(GL_ARRAY_BUFFER, 0, cloth.getSizeOfVertices() * sizeof(glm::vec3), cloth.vertices.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloth.getEBOID());

	glm::mat4 matrix(1.0f);

	//Get Camera State for MVP transfer etc
	glm::vec3 cameraPos = camera->getCameraPos();
	glm::mat4 mvp = camera->getMVP(WIDTH, HEIGHT, matrix);
	glm::mat4 modelView = glm::transpose(matrix);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.01f, far_plane = 50.0f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), lightUp);
	lightSpaceMatrix = lightProjection * lightView;

	GLint shadowShaderID = cloth.getShadowShaderID();
	glUseProgram(shadowShaderID);

	/*
		In Vertex Shader

		layout 0 = aPos
		layout 1 = aColor <- not used
		layout 2 = aTex
		layout 3 = aNormal
	*/
	int aPos = glGetAttribLocation(shadowShaderID, "aPos");
	int aTex = glGetAttribLocation(shadowShaderID, "aTex");
	int aNormal = glGetAttribLocation(shadowShaderID, "aNormal");

	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cloth.getSizeOfVertices() * sizeof(glm::vec3)));

	glEnableVertexAttribArray(aTex);
	glVertexAttribPointer(aTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((cloth.getSizeOfVertices() + cloth.getSizeOfNormals()) * sizeof(glm::vec3)));

	int MVP = glGetUniformLocation(shadowShaderID, "MVP");
	int model = glGetUniformLocation(shadowShaderID, "model");
	int LSM = glGetUniformLocation(shadowShaderID, "lightSpaceMatrix");
	int viewPos = glGetUniformLocation(shadowShaderID, "viewPos");

	glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &modelView[0][0]);
	glUniformMatrix4fv(LSM, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

	glDrawElements(GL_TRIANGLES, cloth.getSizeOfElements(), GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void drawFBOMeshes()
{
	/*
		Rendering for Frame Texture.
		This Texture will be controlled by TEXID variable.
	*/

	//Bind Frame Buffer Object
	glBindFramebuffer(GL_FRAMEBUFFER, FBOID);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Bind VAO
	glBindVertexArray(VAOID);
	drawFBOCloth();
	drawFBOPlane();
	drawFBOSphere();
	glBindVertexArray(0);
	
	//We've got Frame Scene Texture. So UnBind Frame Buffer Object
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void drawRealSphere()
{
	glBindBuffer(GL_ARRAY_BUFFER, sphere.getVBOID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.getEBOID());

	glm::mat4 matrix(1.0f);
	matrix = glm::scale(matrix, glm::vec3(sphere.getRadius()));
	matrix[0][3] = sphere.getPos().x; //glm::translate is not working... why?
	matrix[1][3] = sphere.getPos().y;
	matrix[2][3] = sphere.getPos().z;

	//Get Camera State for MVP transfer etc
	glm::vec3 cameraPos = camera->getCameraPos();
	glm::mat4 mvp = camera->getMVP(WIDTH, HEIGHT, matrix);
	glm::mat4 modelView = glm::transpose(matrix);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.01f, far_plane = 50.0f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), lightUp);
	lightSpaceMatrix = lightProjection * lightView;

	GLint shaderID = sphere.getShaderID();
	glUseProgram(shaderID);

	/*
		In Vertex Shader

		layout 0 = aPos
		layout 1 = aColor <- not used
		layout 2 = aTex <- not used
		layout 3 = aNormal
	*/
	int aPos = glGetAttribLocation(shaderID, "aPos");
	int aNormal = glGetAttribLocation(shaderID, "aNormal");

	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	int MVP = glGetUniformLocation(shaderID, "MVP");
	int model = glGetUniformLocation(shaderID, "model");
	int viewPos = glGetUniformLocation(shaderID, "viewPos");
	int texLoc = glGetUniformLocation(shaderID, "tex");
	int depLoc = glGetUniformLocation(shaderID, "depTex");
	int LSM = glGetUniformLocation(shaderID, "lightSpaceMatrix");

	glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &modelView[0][0]);
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4fv(LSM, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glUniform1i(depLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEXID);

	glDrawElements(GL_TRIANGLES, sphere.getSizeOfElements(), GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void drawRealPlane()
{
	glBindBuffer(GL_ARRAY_BUFFER, plane.getVBOID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.getEBOID());

	glm::mat4 matrix(1.0f);
	
	//Get Camera State for MVP transfer etc
	glm::vec3 cameraPos = camera->getCameraPos();
	glm::mat4 mvp = camera->getMVP(WIDTH, HEIGHT, matrix);
	glm::mat4 modelView = glm::transpose(matrix);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.01f, far_plane = 50.0f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), lightUp);
	lightSpaceMatrix = lightProjection * lightView;

	GLint shaderID = plane.getShaderID();
	glUseProgram(shaderID);

	/*
		In Vertex Shader

		layout 0 = aPos
		layout 1 = aColor <- not used
		layout 2 = aTex <- not used
		layout 3 = aNormal
	*/
	int aPos = glGetAttribLocation(shaderID, "aPos");
	int aNormal = glGetAttribLocation(shaderID, "aNormal");

	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(plane.getSizeOfVertices() * sizeof(glm::vec3)));

	int MVP = glGetUniformLocation(shaderID, "MVP");
	int model = glGetUniformLocation(shaderID, "model");
	int viewPos = glGetUniformLocation(shaderID, "viewPos");
	int texLoc = glGetUniformLocation(shaderID, "tex");
	int depLoc = glGetUniformLocation(shaderID, "depTex");
	int LSM = glGetUniformLocation(shaderID, "lightSpaceMatrix");

	glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &modelView[0][0]);
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4fv(LSM, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glUniform1i(depLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEXID);

	glDrawElements(GL_TRIANGLES, plane.getSizeOfElements(), GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void drawRealCloth()
{
	glBindBuffer(GL_ARRAY_BUFFER, cloth.getVBOID());
	glBufferSubData(GL_ARRAY_BUFFER, 0, cloth.getSizeOfVertices() * sizeof(glm::vec3), cloth.vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, cloth.getSizeOfVertices() * sizeof(glm::vec3), cloth.getSizeOfNormals() * sizeof(glm::vec3), cloth.normals.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloth.getEBOID());

	glm::mat4 matrix(1.0f);

	//Get Camera State for MVP transfer etc
	glm::vec3 cameraPos = camera->getCameraPos();
	glm::mat4 mvp = camera->getMVP(WIDTH, HEIGHT, matrix);
	glm::mat4 modelView = glm::transpose(matrix);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.01f, far_plane = 50.0f;
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), lightUp);
	lightSpaceMatrix = lightProjection * lightView;

	GLint shaderID = cloth.getShaderID();
	glUseProgram(shaderID);

	/*
		In Vertex Shader

		layout 0 = aPos
		layout 1 = aColor <- not used
		layout 2 = aTex
		layout 3 = aNormal
	*/
	int aPos = glGetAttribLocation(shaderID, "aPos");
	int aTex = glGetAttribLocation(shaderID, "aTex");
	int aNormal = glGetAttribLocation(shaderID, "aNormal");

	glEnableVertexAttribArray(aPos);
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(aNormal);
	glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cloth.getSizeOfVertices() * sizeof(glm::vec3)));

	glEnableVertexAttribArray(aTex);
	glVertexAttribPointer(aTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((cloth.getSizeOfVertices() + cloth.getSizeOfNormals()) * sizeof(glm::vec3)));

	int MVP = glGetUniformLocation(shaderID, "MVP");
	int model = glGetUniformLocation(shaderID, "model");
	int viewPos = glGetUniformLocation(shaderID, "viewPos");
	int texLoc = glGetUniformLocation(shaderID, "tex");
	int depLoc = glGetUniformLocation(shaderID, "depTex");
	int LSM = glGetUniformLocation(shaderID, "lightSpaceMatrix");

	glUniformMatrix4fv(MVP, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(model, 1, GL_FALSE, &modelView[0][0]);
	glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniformMatrix4fv(LSM, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cloth.getTextureID());

	glUniform1i(depLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEXID);

	glDrawElements(GL_TRIANGLES, cloth.getSizeOfElements(), GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void drawRealMeshes()
{
	//Bind VAO
	glBindVertexArray(VAOID);
	drawRealCloth();
	drawRealSphere();
	drawRealPlane();
	glBindVertexArray(0);
}
void display()
{
	glCullFace(GL_FRONT);
	drawFBOMeshes(); //Draw FBO
	glCullFace(GL_BACK);

	glClearColor(0.5f, 0.5f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Ready for drawing non-Frame Buffer Meshes.
	drawRealMeshes(); //Draw non-Frame Buffer Meshes

	glutSwapBuffers();
}
