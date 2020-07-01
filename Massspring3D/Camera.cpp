#include "pch.h"
#include "Camera.h"


Camera::Camera(float posX, float posY, float posZ)
{
	cameraPos.x = posX; cameraPos.y = posY; cameraPos.z = posZ;
	fwd.x = 0; fwd.y = 0; fwd.z = 1;
	rht.x = 1; rht.x = 0; rht.x = 0;
	up.x = 0; up.y = 1; up.z = 0;

	cameraDistance = sqrt(posX * posX + posY * posY + posZ * posZ);

}
glm::vec3 Camera::getCameraPos()
{
	return cameraPos;
}
void Camera::setCameraPos(glm::vec3 newCameraPos)
{
	cameraPos = newCameraPos;
}
glm::vec3 Camera::getFWD()
{
	return fwd;
}
void Camera::setFWD(glm::vec3 newFWD)
{
	fwd = newFWD;
}
glm::vec3 Camera::getRHT()
{
	return rht;
}
void Camera::setRHT(glm::vec3 newRHT)
{
	rht = newRHT;
}
glm::vec3 Camera::getUP()
{
	return up;
}
void Camera::setUP(glm::vec3 newUP)
{
	up = newUP;
}
void Camera::addFOV(float p)
{
	if (fov < 45)
		fov += p;
	else //limitation of Field of View
		fov = 45;
}
void Camera::subFOV(float p)
{
	if (fov > 1)
		fov -= p;
	else //limitation of Field of View
		fov = 1;
}
float Camera::getFOV()
{
	return fov;
}
void Camera::subYAW(float p)
{
	//I fogot YAW
	diffX -= p;
	if (abs(diffX) >= PI2)
		diffX = 0;
}
void Camera::subPITCH(float p)
{
	//I fogot PITCH
	diffY -= p;
	if (abs(diffY) >= PI2)
		diffY = 0;
}
float Camera::getYAW()
{
	return diffX;
}
float Camera::getPITCH()
{
	return diffY;
}
void Camera::setFOV(float pf)
{
	fov = pf;
}