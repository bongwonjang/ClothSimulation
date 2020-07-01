#include "pch.h"
#include "Sphere.h"

Sphere::Sphere()
{
	centerPos = glm::vec3(0.0f, -2.0f, 3.0f);
}
Sphere::~Sphere()
{
	vertices.clear();
	normals.clear();
	elements.clear();
}

bool Sphere::loadOBJ(const char * objPath)
{
	char count[128];
	int vertexNum = 0;
	int faceNum = 0;

	FILE * fp = fopen(objPath, "r");

	if (!fp)
	{
		std::cout << "Can't open" << std::endl;
		return false;
	}

	float vx1, vy1, vz1;
	int x1, x2, y1, y2, z1, z2;

	while (!feof(fp))
	{
		fscanf(fp, "%s", count);
		if (strcmp(count, "v") == 0)
		{
			fscanf(fp, "%f %f %f\n", &vx1, &vy1, &vz1);
			vertices.push_back(glm::vec3(vx1, vy1, vz1));
		}
		else if (strcmp(count, "vn") == 0)
		{
			fscanf(fp, "%f %f %f\n", &vx1, &vy1, &vz1);
			normals.push_back(glm::vec3(vx1, vy1, vz1));
		}
		else if (strcmp(count, "f") == 0)
		{
			/*
			The sphere obj that I'm using doesn't have texture coord.
			*/
			fscanf(fp, "%d//%d %d//%d %d//%d\n", &x1, &x2, &y1, &y2, &z1, &z2);

			//1 ; vetex 2 : vertex normal
			x1--; y1--; z1--; //POSITION x y z w
			x2--; y2--; z2--; //NORMAL x y z

			elements.push_back(x1);
			elements.push_back(y1);
			elements.push_back(z1);

			elements.push_back(0);
			elements.push_back(0);
			elements.push_back(0);
		}
	}

	fclose(fp);

	return true;


}
bool Sphere::generateVBOEBO()
{
	//Initialze VBO for this mesh
	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);

	glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());
	
	glGenBuffers(1, &EBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, elements.size() * sizeof(unsigned int), elements.data());

	if (VBOID == 0 || EBOID == 0)
		return false;

	std::cout << "VBOID: " << VBOID << " EBOID: " << EBOID << std::endl;
	return true;
}
std::string Sphere::readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}
void Sphere::loadShader(const char * vertexShaderPath, const char * fragmentShaderPath, int mode) {

	if (vertexShaderPath == NULL || fragmentShaderPath == NULL)
	{
		std::cout << "Can't Load Shader" << std::endl;
		return;
	}

	GLuint vertShader = createShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragShader = createShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	std::cout << "vertexShader: " << vertShader << " fragShader: " << fragShader << std::endl;

	if (mode == 0)
	{
		//Normal mode
		shaderID = glCreateProgram();

		glAttachShader(shaderID, vertShader);
		glAttachShader(shaderID, fragShader);

		glBindAttribLocation(shaderID, 0, "aPos");
		glBindAttribLocation(shaderID, 1, "aColor");
		glBindAttribLocation(shaderID, 2, "aTex");
		glBindAttribLocation(shaderID, 3, "aNormal");

		// Link Shader - 마치 obj파일들을 묶어서 .exe 파일로
		glLinkProgram(shaderID);
	}
	else
	{
		//Depth mode
		shadowShaderID = glCreateProgram();

		glAttachShader(shadowShaderID, vertShader);
		glAttachShader(shadowShaderID, fragShader);

		glBindAttribLocation(shadowShaderID, 0, "aPos");
		glBindAttribLocation(shadowShaderID, 1, "aColor");
		glBindAttribLocation(shadowShaderID, 2, "aTex");
		glBindAttribLocation(shadowShaderID, 3, "aNormal");

		// Link Shader - 마치 obj파일들을 묶어서 .exe 파일로
		glLinkProgram(shadowShaderID);
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	//programID를 사용하는 것은 main이 있는 cpp에서
	std::cout << "Shader Loaded" << std::endl;
}
unsigned int Sphere::createShader(const char * path, GLuint shaderMode)
{
	if (path == NULL)
	{
		std::cout << "No Shader Path" << std::endl;
		return -1;
	}
	GLuint shader = glCreateShader(shaderMode);

	// Read shaders
	std::string shaderStr = readFile(path);
	const char *shaderSrc = shaderStr.c_str();

	// Compile vertex shader - 마치 .obj 파일로
	std::cout << "Compiling shader." << std::endl;
	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);

	return shader;
}
unsigned int Sphere::getVBOID()
{
	return VBOID;
}
unsigned int Sphere::getEBOID()
{
	return EBOID;
}
int Sphere::getShaderID()
{
	return shaderID;
}
int Sphere::getShadowShaderID()
{
	return shadowShaderID;
}
int Sphere::getSizeOfNormals()
{
	return normals.size();
}
int Sphere::getSizeOfVertices()
{
	return vertices.size();
}
int Sphere::getSizeOfElements()
{
	return elements.size();
}
float Sphere::getRadius()
{
	return radius;
}
glm::vec3 Sphere::getPos()
{
	return centerPos;
}
void Sphere::setPos(glm::vec3 newPos)
{
	centerPos = newPos;
}