#ifndef CAMERA_H
#define CAMERA_H
#include <glad/gl.h>
#include "AlexAlgebra.h"



class Camera
{
public:
	aa::vec3 cameraPos = aa::vec3(0.0f, 0.0f, 3.0f);
	aa::vec3 cameraFront = aa::vec3(0.0f, 0.0f, -1.0f);
	aa::vec3 cameraUp = aa::vec3(0.0f, 1.0f, 0.0f);

	float yaw = -90.0f;
	float pitch = 0.0f;
	float zoom = 45.0f;  // for fps camera
	float radius = 5.0f; // for orbiting camera

	int windowWidth;
	int windowHeight;

	bool orbitingCamera = true;

	Camera(int windowWidth = 800.0f, int windowHeight = 600.0f);
	aa::mat4 view();
	aa::mat4 projection(float e = 0.0f);
	aa::mat4 projectionLeft();
	aa::mat4 projectionRight();
	aa::mat4 inverseViewProjection();
};

#endif
