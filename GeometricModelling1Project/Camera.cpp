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
	//std::cout << cameraPos.z << "\n";
	if (orbitingCamera)
	{
		if (radius < 1.0f)
			radius = 1.0f;
		if (radius > 30.0f)
			radius = 30.0f;
		cameraPos = -cameraFront * radius;
		view = aa::lookAt(cameraPos, aa::vec3(0.0f), cameraUp);
	}
	else
	{
		view = aa::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	return view;
}
aa::mat4 Camera::projection(float e)
{
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
}
/*aa::mat4 Camera::projection(float e)
{
	float near = 0.1f;
	float far = 100.0f;

	float top = near * tan(aa::radians(zoom) / 2.0f);
	float rightPlane = top * ((float)windowWidth / windowHeight);

	float convergence = 5.0f;
	float frustumShift = (e) * near / convergence;

	aa::mat4 projection = aa::frustum(
		-rightPlane - frustumShift,
		rightPlane - frustumShift,
		-top,
		top,
		near,
		far
	);
	return projection;
}*/
aa::mat4 Camera::projectionLeft()
{
	return projection(0.05f);
}
aa::mat4 Camera::projectionRight()
{
	return projection(-0.05f);
}
aa::mat4 Camera::inverseViewProjection()
{
	aa::mat4 VP = projection() * view();
	aa::mat4 invVP = aa::inverse(VP);
	return invVP;
}