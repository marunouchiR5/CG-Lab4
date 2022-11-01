#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	eyePosition = glm::vec3(0.0f, 0.0f, -2.0f);

	wireframe = 0;
	paint = 0;
	camera_left = -1.0f;
	camera_right = 1.0f;
	camera_top = 1.0f;
	camera_bottom = -1.0f;
	camera_near = -1.0f;
	camera_far = 1.0f;

	red = 255;
	green = 128;
	blue = 0; // controls the color

	castRay = false;
	mouseX = 0;
	mouseY = 0;
	spherePosition = glm::vec3(0, 0, 0);

	myObject = new SceneObject();
}

MyGLCanvas::~MyGLCanvas() {
}

void MyGLCanvas::setColor(int r, int g, int b) {
	red = r;
	green = g;
	blue = b;
}

glm::vec3 MyGLCanvas::getEyePoint(int pixelX, int pixelY, int screenWidth, int screenHeight) {
	glm::vec3 eye(float(- screenWidth + 2 * pixelX) / float(screenWidth), float(- screenHeight + 2 * pixelY) / float(screenWidth), 0);
	return eye;
}


/* The generateRay function accepts the mouse click coordinates
	(in x and y, which will be integers between 0 and screen width and 0 and screen height respectively).
   The function returns the ray
*/
glm::vec3 MyGLCanvas::generateRay(int pixelX, int pixelY) {
	glm::vec3 ray(0, 0, 1);
	return ray;
}

/* The intersect function accepts three input parameters:
	(1) the eye point (in world coordinate)
	(2) the ray vector (in world coordinate)
	(3) the transform matrix that would be applied to there sphere to transform it from object coordinate to world coordinate

	The function should return:
	(1) a -1 if no intersection is found
	(2) OR, the "t" value which is the distance from the origin of the ray to the (nearest) intersection point on the sphere
*/
double MyGLCanvas::intersect (glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix) {
	double t = -1;

	glm::vec4 eyePointPO = glm::inverse(transformMatrix) * glm::vec4(eyePointP, 1);
	glm::vec4 d = glm::inverse(transformMatrix) * glm::vec4(rayV, 0);
	// glm::vec4 d = glm::normalize(rayVO);     

	float r = 0.5;
	float a = glm::dot(d, d);
	float b = 2 * glm::dot(eyePointPO, d);
	float c = glm::dot(eyePointPO, eyePointPO) - r * r;
	double delta = b * b - 4 * a * c;

	if (delta < 0) {
		return t;
	}
	else {
		return std::min((-b + pow(delta, 0.5)) / (2 * a), (-b - pow(delta, 0.5)) / (2 * a));
	}

	return t;
}


/* The getIsectPointWorldCoord function accepts three input parameters:
	(1) the eye point (in world coordinate)
	(2) the ray vector (in world coordinate)
	(3) the "t" value

	The function should return the intersection point on the sphere
*/
glm::vec3 MyGLCanvas::getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t) {
	glm::vec3 p(0.0);
	p = eye + t * ray;
	return p;
}


void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context");

		// Set the base texture of our object. Note that loading gl texture can 
		//  only happen after the gl context has been established
		if (myObject->baseTexture == NULL) {
			myObject->setTexture(0, "./data/pink.ppm");
		}
		// Set a second texture layer to our object
		if (myObject->blendTexture == NULL) {
			myObject->setTexture(1, "./data/smile.ppm");
		}

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		//glShadeModel(GL_SMOOTH);
		glShadeModel(GL_FLAT);

		GLfloat light_pos0[] = { eyePosition.x, eyePosition.y, eyePosition.z, 0.0f };
		GLfloat ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
		GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };

		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);

		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		/**********************************************/
		/*    Enable normalizing normal vectors       */
		/*    (e.g. normals not affected by glScalef) */
		/**********************************************/

		glEnable(GL_NORMALIZE);

		/****************************************/
		/*          Enable z-buferring          */
		/****************************************/

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawScene();
}

void MyGLCanvas::drawScene() {

	if (castRay == true) {
		int screenWidth = w();
		int screenHeight = h();

		glm::vec3 rayOriginPoint = getEyePoint(mouseX, mouseY, screenWidth, screenHeight);
		glm::vec3 rayV = generateRay(mouseX, mouseY);
		glm::vec3 sphereTransV(spherePosition[0], spherePosition[1], spherePosition[2]);
		float t = intersect(rayOriginPoint, rayV, glm::translate(glm::mat4(1.0), sphereTransV));

		glm::vec3 isectPointWorldCoord = getIsectPointWorldCoord(rayOriginPoint, rayV, t);

		std::cout << "isect: " << isectPointWorldCoord.x << ", " << isectPointWorldCoord.y << std::endl;
		std::cout << "length: " << glm::length(isectPointWorldCoord) << std::endl;

		if (t > 0) {
			if (paint == 1) {
				myObject->paintTexture(isectPointWorldCoord, red, green, blue);
			}
			else {
				glColor3f(1, 0, 0);
				glPushMatrix();
				glTranslated(spherePosition[0], spherePosition[1], spherePosition[2]);
				glutWireCube(1.0f);
				glPopMatrix();
				glPushMatrix();
				glTranslatef(isectPointWorldCoord[0], isectPointWorldCoord[1], isectPointWorldCoord[2]);
				glutSolidSphere(0.05f, 10, 10);
				glPopMatrix();
				printf("hit!\n");
			}
		}
		else {
			printf("miss!\n");
		}
	}

	glPushMatrix();

	//move the sphere to the designated position
	glTranslated(spherePosition[0], spherePosition[1], spherePosition[2]);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glColor3f(1.0, 1.0, 1.0);
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		myObject->drawTexturedSphere();
	glPopMatrix();

	glPopMatrix();
}


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

// Determine if we are modifying the camera(GL_PROJECITON) matrix(which is our viewing volume)
	// Otherwise we could modify the object transormations in our world with GL_MODELVIEW
	glMatrixMode(GL_PROJECTION);
	// Reset the Projection matrix to an identity matrix
	glLoadIdentity();

	glOrtho(camera_left, camera_right, camera_bottom, camera_top, camera_near, camera_far);
}


int MyGLCanvas::handle(int e) {
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_MOVE:
		Fl::belowmouse(this);
		//printf("mouse move event (%d, %d)\n", (int)Fl::event_x(), (int)Fl::event_y());
		mouseX = (int)Fl::event_x();
		mouseY = (int)Fl::event_y();

		break;
	case FL_DRAG:
		printf("mouse drag\n");
		if ((Fl::event_button() == FL_LEFT_MOUSE) && (castRay == true)) { //left mouse drag
			castRay = true;
			mouseX = (int)Fl::event_x();
			mouseY = (int)Fl::event_y();
		}
		return (1);
	case FL_PUSH:
		printf("mouse push\n");
		if ((Fl::event_button() == FL_LEFT_MOUSE) && (castRay == false)) { //left mouse click
			castRay = true;
		}
		return (1);
	case FL_RELEASE:
		printf("mouse release\n");
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			castRay = false;
		}
		return (1);
	}

	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::drawAxis() {
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, 0, 0); glVertex3f(0.0, 1.0, 0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
	glEnd();
	glEnable(GL_LIGHTING);
}