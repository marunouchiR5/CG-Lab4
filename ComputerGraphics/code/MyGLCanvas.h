#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <time.h>
#include <iostream>

#include "SceneObject.h"

class MyGLCanvas : public Fl_Gl_Window {
public:

	// Length of our spline (i.e how many points do we randomly generate)
	int wireframe;
	int paint;
	int red, green, blue;
	glm::vec3 eyePosition;

	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();

	void setColor(int r, int g, int b);

private:
	glm::vec3 generateRay(int pixelX, int pixelY);
	glm::vec3 getEyePoint(int pixelX, int pixelY, int screenWidth, int screenHeight);
	glm::vec3 getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t);
	double intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);

	void draw();
	void drawScene();

	void drawAxis();
	void drawGrid();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);

	SceneObject* myObject;
	bool castRay;

	float camera_left;
	float camera_right;
	float camera_top;
	float camera_bottom;
	float camera_near;
	float camera_far;

	//// Used for intersection
	glm::vec3 spherePosition;

	int mouseX;
	int mouseY;
};

#endif // !MYGLCANVAS_H