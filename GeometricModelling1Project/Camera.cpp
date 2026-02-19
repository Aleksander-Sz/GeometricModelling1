#include "Camera.h"

Camera::Camera(int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}
glm::mat4 Camera::view()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}
/*glm::mat4 Camera::projection(float e)
{
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	float r = 5.0f;
	projection = glm::mat4(glm::vec4(1.0f,0.0f,0.0f,0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(-e/2/r, 0.0f, 0.0f, -1.0f/r), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	return projection;
}*/
glm::mat4 Camera::projection(float e)
{
	float near = 0.1f;
	float far = 100.0f;

	float top = near * tan(glm::radians(zoom) / 2.0f);
	float rightPlane = top * ((float)windowWidth / windowHeight);

	float convergence = 5.0f;
	float frustumShift = (e) * near / convergence;

	glm::mat4 projection = glm::frustum(
		-rightPlane - frustumShift,
		rightPlane - frustumShift,
		-top,
		top,
		near,
		far
	);
	return projection;
}
glm::mat4 Camera::projectionLeft()
{
	return projection(0.05f);
}
glm::mat4 Camera::projectionRight()
{
	return projection(-0.05f);
}
glm::mat4 Camera::inverseViewProjection()
{
	glm::mat4 VP = projection() * view();
	glm::mat4 invVP = glm::inverse(VP);
	return invVP;
}