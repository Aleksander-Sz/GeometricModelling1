#include "Camera.h"
#include <iostream>
Camera::Camera(int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}
aa::mat4 Camera::view()
{
	aa::mat4 view = aa::mat4(1.0f);
	aa::vec3 direction;
	direction.x = cos(aa::radians(yaw)) * cos(aa::radians(pitch));
	direction.y = sin(aa::radians(pitch));
	direction.z = sin(aa::radians(yaw)) * cos(aa::radians(pitch));
	cameraFront = aa::normalize(direction);
	aa::vec3 cameraRight = aa::normalize(aa::cross(cameraFront, cameraUp));
	if (orbitingCamera)
	{
		if (radius < 1.0f)
			radius = 1.0f;
		if (radius > 30.0f)
			radius = 30.0f;
		cameraPos = orbitingCameraTarget - cameraFront * radius;
		aa::vec3 eyePosition = cameraPos + cameraRight * eyeOffset;
		view = aa::lookAt(eyePosition, eyePosition + cameraFront, cameraUp);
	}
	else
	{
		aa::vec3 eyePosition = cameraPos + cameraRight * eyeOffset;
		view = aa::lookAt(eyePosition, eyePosition + cameraFront, cameraUp);
	}
	return view;
}
aa::mat4 Camera::projection()
{
	float f = 10000.0f;
	float n = 0.1f;
	aa::mat4 projection;
	float aspect = (float)windowWidth / (float)windowHeight;

	float fov = aa::radians(zoom);
	float top = n * tan(fov / 2.0f);
	float bottom = -top;

	float right = top * aspect;
	float left = -right;

	float shift = -eyeOffset * n / convergenceDistance;

	right += shift;
	left += shift;
	float A1 = 2.0f * n / (right - left);
	float B2 = 2.0f * n / (top - bottom);
	float A3 = (right + left) / (right - left);
	float B3 = (top + bottom) / (top - bottom);
	float C3 = (f + n) / (f - n);
	float C4 = (2.0f * f * n) / (f - n);
	projection = aa::mat4(aa::vec4(A1, 0.0f, 0.0f, 0.0f), aa::vec4(0.0f, B2, 0.0f, 0.0f), aa::vec4(A3, B3, -C3, -1.0f), aa::vec4(0.0f, 0.0f, -C4, 0.0f));
	return projection;
}
/*{
	aa::mat4 projection;
	float aspect = (float)windowWidth / (float)windowHeight;
	float fov = aa::radians(zoom);
	float f = 10000.0f;
	float n = 0.1f;
	float A = 1.0f / std::tan(fov / 2.0f) / aspect;
	float B = 1.0f / std::tan(fov / 2.0f);
	float C = -(f + n) / (f - n);
	float D = -(2.0f * f * n) / (f - n);
	projection = aa::mat4(aa::vec4(A,0.0f,0.0f,0.0f), aa::vec4(0.0f, B, 0.0f, 0.0f), aa::vec4(0.0f, 0.0f, C, -1.0f), aa::vec4(0.0f, 0.0f, D, 0.0f));
	return projection;
}*/
aa::mat4 Camera::inverseViewProjection()
{
	aa::mat4 VP = projection() * view();
	aa::mat4 invVP = aa::inverse(VP);
	return invVP;
}