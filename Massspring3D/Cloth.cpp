#include "pch.h"
#include "Cloth.h"
#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"

Cloth::Cloth()
{
}
Cloth::~Cloth()
{
	vertices.clear();
	normals.clear();
	textures.clear();
	elements.clear();
	
	delete[] force, accel, velocity;
}

void Cloth::init(float stiffness)
{
	k = stiffness;
	force = new glm::vec3[numX * numY]();
	accel = new glm::vec3[numX * numY]();
	velocity = new glm::vec3[numX * numY]();

	for (int i = 0; i < numX; i++)
	{
		for (int j = 0; j < numY; j++)
		{
			//Initialzie force
			force[j + numY * i].x = 0.0f;
			force[j + numY * i].y = 0.0f;
			force[j + numY * i].z = 0.0f;

			//Initialzie velocity
			velocity[j + numY * i].x = 0.0f;
			velocity[j + numY * i].y = 0.0f;
			velocity[j + numY * i].z = 0.0f;
		}
	}
}
glm::vec3 Cloth::forceCal(int target, int source, float originalDis, float factor)
{
	/*
		Source is me, Target is you
		originalDis is the original distance between nodes 
	*/
	glm::vec3 dir;
	glm::vec3 springForce;
	glm::vec3 dampingForce;

	dir = vertices[source] - vertices[target]; //Current force(not spring force) direction
	float abs = glm::length(dir);
	dir = glm::normalize(dir);

	springForce = factor * -k * (abs - originalDis) * dir; //opposite direction of Current force
	dampingForce = -c * glm::dot(dir, velocity[source] - velocity[target]) * dir;

	return springForce + dampingForce;
}
void Cloth::update(glm::vec3 planeNormalVec, float planeD, glm::vec3 spherePos, float radius)
{
	/*
		Calculate node's force and change the node's velocity and postion
		Left, Right, Top, Bottom meaning depend on how the cloth nodes are organized.
	*/
	for (int i = 0; i < numX * numY; i++)
	{
		glm::vec3 tempForce(0.0f);

		/*
			Neighbor force
			o-o
		*/

		if (i % numX != 0)//Not left edge
			tempForce = tempForce + forceCal(i - 1, i, dx, 1.0f);

		if (i % numX != numY - 1)//Not right edge
			tempForce = tempForce + forceCal(i + 1, i, dx, 1.0f);

		if (i >= numX)//Not top edge
			tempForce = tempForce + forceCal(i - numX, i, dx, 1.0f);

		if (i <= (numY - 1) * numX - 1)//Not bottom edge
			tempForce = tempForce + forceCal(i + numX, i, dx, 1.0f);
		
		/*
			Shear force
			o
			 \
			  o
		*/
		if (i % numX != 0 && i >= numX) //Not left-top edge
			tempForce = tempForce + forceCal(i - numX - 1, i, dx * 1.414f, 1.0f);

		if (i % numX != numY - 1 && i >= numX) //Not right-top edge
			tempForce = tempForce + forceCal(i - numX + 1, i, dx * 1.414f, 1.0f);

		if (i % numX != 0 && i <= (numY - 1) * numX - 1) //Not left-bottom edge
			tempForce = tempForce + forceCal(i + numX - 1, i, dx * 1.414f, 1.0f);

		if (i % numX != numY - 1 && i <= (numY - 1) * numX - 1) //Not right-bottom edge
			tempForce = tempForce + forceCal(i + numX + 1, i, dx * 1.414f, 1.0f);

		/*
			bend force
			o--o
		*/
		if (i % numX > 1)//Not left or left+1 edge
			tempForce = tempForce + forceCal(i - 2, i, dx * 2.0f, 1.0f);

		if (i % numX < numY - 2)//Not right or right-1 edge
			tempForce = tempForce + forceCal(i + 2, i, dx * 2.0f, 1.0f);

		if (i >= numX * 2)//Not top or top+1 edge
			tempForce = tempForce + forceCal(i - 2 * numX, i, dx * 2.0f, 1.0f);

		if (i <= (numY - 2) * numX - 1)//Not bottom or bottom-1 edge
			tempForce = tempForce + forceCal(i + 2 * numX, i, dx * 2.0f, 1.0f);

		//Combine all foreces
		force[i] = force[i] + tempForce;
	}

	/*
		Calculate new velcotiy and new positon.
	*/
	for (int i = 0; i < numX * numY; i++)
	{
		/*
			isHold means if the cloth object is held
			I made a mistake! Hold -> Held.
		*/
		if (isHold && (i == 0 || i == numX - 1))
		{
			/*
				Never change the acceleration, velocity, position of cloth's tips.
				They are pinned
			*/
			accel[i] = glm::vec3(0.0f);
			velocity[i] = glm::vec3(0.0f);
			vertices[i] = vertices[i];
		}
		else
		{
			/*
				Previous Euler method:

				accel[i] = force[i] / m - glm::vec3(0.0f, 9.8f, 0.0f);
				velocity[i] = velocity[i] + accel[i] * dt;
				vertices[i] = vertices[i] + velocity[i] * dt;

			*/

			/*
				4th Runge-Kutta:
					Is it correct?
					It looks working well.
			*/
			glm::vec3 p1 = vertices[i];
			glm::vec3 v1 = velocity[i];
			glm::vec3 a1 = accel[i] = force[i] / m - glm::vec3(0.0f, 9.8f, 0.0f);

			glm::vec3 p2 = p1 + v1 * dt / 2.0f;
			glm::vec3 v2 = v1 + a1 * dt / 2.0f;
			glm::vec3 a2 = (v2 - v1) / dt;

			glm::vec3 p3 = p1 + v2 * dt / 2.0f;
			glm::vec3 v3 = v1 + a2 * dt / 2.0f;
			glm::vec3 a3 = (v3 - v2) / dt;

			glm::vec3 p4 = p1 + v3 * dt;
			glm::vec3 v4 = v1 + a3 * dt;
			glm::vec3 a4 = (v4 - v3) / dt;

			velocity[i] += (a1 + 2.0f * a2 + 2.0f * a3 + a4) * dt / 6.0f;
			vertices[i] += (v1 + 2.0f * v2 + 2.0f * v3 + v4) * dt / 6.0f;


			/*
				Sphere and Cloth collision.
				Plane and Cloth collision. 

				Sphere and Plane don't move. So we care only cloth's collision handling.

				The way to handle collision is simple.
				Think each node in the cloth is a small sphere(radius = 0.1f).
				Then, we can apply Sphere-Sphere Collision and Sphere-Plane Collision simulation.

				However, there is a problem.
				When handling the collision, the cloth's nodes are vibrating.
			*/
			float interval = glm::length(vertices[i] - spherePos);
			if (interval < radius + 0.1f)
			{
				//just stop the cloth's node.
				velocity[i] = glm::vec3(0.0f);
				//0.1f is margin
				vertices[i] += (0.1f + radius - interval) * glm::normalize(vertices[i] - spherePos);
			}
			else if (radius + 0.1f <= interval && interval <= radius + 0.3f)
			{
				velocity[i] = glm::vec3(0.0f);
			}

			float param = glm::length(glm::dot(planeNormalVec, vertices[i]) + planeD);
			if (param < 0.1f)
			{
				//just stop the cloth's node
				velocity[i] = glm::vec3(0.0f);
				//0.1f is margin
				vertices[i] += 0.1f * planeNormalVec;
			}
			else if (0.1f <= param && param <= 0.3f)
			{
				velocity[i] = glm::vec3(0.0f);
			}
		}

		//Clear the force value so that previous force value doesn't affect the next value
		force[i].x = 0.0f;
		force[i].y = 0.0f;
		force[i].z = 0.0f;
	}

	updateNormal();
}
void Cloth::updateNormal()
{
	/*
		I have to update Normal vector every time!
		It works fine, and the rendered image looks fine.
	*/
	unsigned int index = 0;
	for (int yIdx = 0; yIdx < int(numY); yIdx++)
	{
		for (int xIdx = 0; xIdx < int(numX); xIdx++)
		{
			if (xIdx < numX - 1 && yIdx < numY - 1)
			{
				/*
					o's normal vector = newNormal
					n's normal vector is not updated with the newNormal's value
					
					o---o
					|\  |
					| \ |
					n---o
				*/
				glm::vec3 newNormal = glm::cross(vertices[index + numX + 1] - vertices[index],
												 vertices[index + 1] - vertices[index]);
				normals[index] = newNormal;
				normals[index + 1] = newNormal;
				normals[index + numX + 1] = newNormal;

				/*
					o's normal vector = newNormal
					n's normal vector is not updated with the newNormal's value

					n---n
					|\  |
					| \ |
					o---n
				*/
				newNormal = glm::cross(vertices[index + numX] - vertices[index],
					vertices[index + numX + 1] - vertices[index]);
				normals[index + numX] = newNormal;
			}
			index++;
		}
	}
}
void Cloth::changeIsHold()
{
	isHold = !isHold;
}

void Cloth::control()
{
	/*
		Give big force to the bottom edge of the cloth.
		The direction is z-axis.
		10000.0f is enough
	*/
	int index = numX * numY - numX;
	for(int i = index; i < numX * numY; i++)
		force[i] = force[index] + glm::vec3(0.0f, 0.0f, 10000.0f);
}
void Cloth::createClothMesh(float p_numX, float p_numY)
{
		
	/*
		I don't think you can understand this explanation...
		
		Z-axis value is 0
		if cloth is 7 x 7 then,
		
			|----------numX---------|
		index start ->
			(-1, 1)                (1, 1)
		=	0 --1 --2 --3 --4 --5 --6
		|	|						|
		|	7						13
		n	|			(0, 0)		|
		u	14						20
		m	|						|
		Y	21--22--23--24--25--26--27
		|   (-1, -1)	   		   (1, -1)
		=				    -> index end
	*/

	numX = p_numX;
	numY = p_numY;
	
	float xInterval = clothSize / numX;
	dx = xInterval;

	float yInterval = clothSize / numY;
	unsigned int index = 0;

	float halvedClothSize = clothSize / 2;

	for (int yIdx = 0; yIdx < int(numY); yIdx++)
	{
		for (int xIdx = 0; xIdx < int(numX); xIdx++)
		{
			vertices.push_back(glm::vec3(-halvedClothSize + xInterval * xIdx, halvedClothSize - yInterval * yIdx + 5.0f, 0.0f));
			textures.push_back(glm::vec2(-1.0f + xInterval * xIdx / 5, 1.0f - yInterval * yIdx / 5));
			normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

			if (xIdx < numX - 1 && yIdx < numY - 1)
			{
				elements.push_back(index);
				elements.push_back(index + numX + 1);
				elements.push_back(index + 1);
				
				elements.push_back(index);
				elements.push_back(index + numX);
				elements.push_back(index + numX + 1);
			}
			
			index++;
		}
	}
	std::cout << "Load Finish\nvertices size: " << vertices.size() << " elements size: " << elements.size() << std::endl;
	std::cout << "Expected size of veritces: " << 256 << "\nExpected size of elements: " << 1350 << std::endl;

}
bool Cloth::generateVBOEBO()
{
	//Initialze VBO for this mesh
	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(glm::vec3) + textures.size() * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(glm::vec3), textures.size() * sizeof(glm::vec2), textures.data());

	glGenBuffers(1, &EBOID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, elements.size() * sizeof(unsigned int), elements.data());

	if (VBOID == 0 || EBOID == 0)
		return false;
	
	std::cout << "VBOID: " << VBOID << " EBOID: " << EBOID << std::endl;
	return true;
}
std::string Cloth::readFile(const char *filePath) {
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
void Cloth::loadShader(const char * vertexShaderPath, const char * fragmentShaderPath, int mode) {

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
unsigned int Cloth::createShader(const char * path, GLuint shaderMode)
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
bool Cloth::loadTexture(const char * texturePath)
{
	/*
	Loading Texture
	*/
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID); //Generate Texture ID
	glBindTexture(GL_TEXTURE_2D, textureID); //Bind Texture ID

	/*
	Setting Texture MipMap Parameters
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	textureData = stbi_load(texturePath, &width, &height, &nrChannels, 0); //Read JPG image for texture

	if (textureData)
	{
		//Creatre Texture Image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Something Error in Texture" << std::endl;
		stbi_image_free(textureData);
		glBindTexture(GL_TEXTURE_2D, 0);
		return false;
	}
	stbi_image_free(textureData);

	//After finishing loading texture, UnBind Texture ID
	glBindTexture(GL_TEXTURE_2D, 0);


	return true;
}
unsigned int Cloth::getVBOID()
{
	return VBOID;
}
unsigned int Cloth::getEBOID()
{
	return EBOID;
}
GLuint Cloth::getTextureID()
{
	return textureID;
}
int Cloth::getShaderID()
{
	return shaderID;
}
int Cloth::getShadowShaderID()
{
	return shadowShaderID;
}
int Cloth::getSizeOfNormals()
{
	return normals.size();
}
int Cloth::getSizeOfVertices()
{
	return vertices.size();
}
int Cloth::getSizeOfElements()
{
	return elements.size();
}
int Cloth::getSizeOfTextures()
{
	return textures.size();
}