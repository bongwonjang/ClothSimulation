#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

class Camera
{
private:
	glm::vec3 cameraPos;
	glm::vec3 fwd;
	glm::vec3 rht;
	glm::vec3 up;

	float cameraDistance = 20;
	float fov = 45;
	float diffX = 0; //mouse <-> camera moving yaw
	float diffY = 0; //mouse <-> camera moving pitch

	const float PI1 = 3.141592f;
	const float PI2 = 6.283184f;

public:
	Camera(float posX, float posY, float posZ);

	glm::vec3 getCameraPos();
	void setCameraPos(glm::vec3 newCameraPos);

	glm::vec3 getFWD();
	void setFWD(glm::vec3 newFWD);

	glm::vec3 getRHT();
	void setRHT(glm::vec3 newRHT);

	glm::vec3 getUP();
	void setUP(glm::vec3 newUP);

	void addFOV(float p);
	void subFOV(float p);
	float getFOV();
	void setFOV(float pf);
	void subYAW(float p);
	void subPITCH(float p);
	float getYAW();
	float getPITCH();

	/*
	Important : based on spehre coordinate system
	*/
	void updateCameraPos()
	{
		fwd.x = -sin(diffX) * cos(diffY);
		fwd.y = sin(diffY);
		fwd.z = -cos(diffX) * cos(diffY);

		rht.x = -cos(diffX);
		rht.y = 0;
		rht.z = sin(diffX);

		up = glm::cross(fwd, rht);
		glm::normalize(fwd);
		glm::normalize(rht);
		glm::normalize(up);

		cameraPos.x = fwd.x * -cameraDistance;
		cameraPos.y = fwd.y * -cameraDistance;
		cameraPos.z = fwd.z * -cameraDistance;

	}
	glm::mat4 getProjection(float width, float height)
	{
		return glm::perspective(glm::radians(fov), (float)(width / height), 0.1f, 100.0f);
	}
	glm::mat4 getView()
	{
		return glm::lookAt(cameraPos, glm::vec3(0, 0, 0), up);
		//return glm::lookAt(cameraPos, fwd, up);
	}
	glm::mat4 getMVP(float width, float height, glm::mat4 matrix)
	{
		glm::mat4 Projection = glm::perspective(glm::radians(fov), (float)(width / height), 0.1f, 100.0f);
		glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), up);

		//We'll use transpose. OpenGL supports column based matrix.
		glm::mat4 Model = glm::transpose(matrix);

		//The order is from right to left.
		return Projection * View  * Model;
	}
};