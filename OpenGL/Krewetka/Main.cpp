//----------------------------------------------------------------------
//	Best if viewed with tab stops set every 2 columns.
//----------------------------------------------------------------------
//	File: opengl-3D-sample.cpp - Sample 3D OpenGL/GLUT program
//	Origin: Lighthouse3D (heavily modified by Dave Mount)
//
//	This is a sample program that illustrates OpenGL and GLUT. It
//	renders a picture of 36 snowmen. The camera can be moved by dragging
//	the mouse. The camera moves forward by hitting the up-arrow key and
//	back by moving the down-arrow key. Hit ESC, 'q' or 'Q' to exit.
//
//	Warning #1: This program uses the function glutSpecialUpFunc, which
//	may not be available in all GLUT implementations. If your system
//	does not have it, you can comment this line out, but the up arrow
//	processing will not be correct.
//
//	Warning #2: This is a minimalist program. Very little attention has
//	been paid to good programming technique.
//----------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h> // standard definitions
#include <math.h> // math definitions
#include <stdio.h> // standard I/O

// include files are in a slightly different location for MacOS
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// escape key (for exit)
#define ESC 27

GLuint LoadBMP(const char *fileName)
{
	FILE *file;
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int size;
	unsigned int width, height;
	unsigned char *data;


	file = fopen(fileName, "rb");

	if (file == NULL){
		printf("brak pliku.");
		return false;
	}

	if (fread(header, 1, 54, file) != 54){
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M'){
		return false;
	}

	dataPos = *(int*)&(header[0x0A]);
	size = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (size == NULL)
		size = width * height * 3;
	if (dataPos == NULL)
		dataPos = 54;

	data = new unsigned char[size];

	fread(data, 1, size, file);

	fclose(file);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return texture;
}
static GLuint texture;

//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y) are the current camera position. The values (lx, ly)
// point in the direction the camera is looking. The variables angle and
// deltaAngle control the camera's angle. The variable deltaMove
// indicates the amount of incremental motion for the camera with each
// redraw cycle. The variables isDragging and xDragStart are used to
// monitor the mouse when it drags (with the left button down).
//----------------------------------------------------------------------

// Camera position
float x = 0.0, y = -5.0; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts

//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
	float ratio = ((float)w) / ((float)h); // window aspect ratio
	glMatrixMode(GL_PROJECTION); // projection matrix is active
	glLoadIdentity(); // reset the projection
	gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
	glMatrixMode(GL_MODELVIEW); // return to modelview mode
	glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw one snowmen (at the origin)
//
// A snowman consists of a large body sphere and a smaller head sphere.
// The head sphere has two black eyes and an orange conical nose. To
// better create the impression they are sitting on the ground, we draw
// a fake shadow, consisting of a dark circle under each.
//
// We make extensive use of nested transformations. Everything is drawn
// relative to the origin. The snowman's eyes and nose are positioned
// relative to a head sphere centered at the origin. Then the head is
// translated into its final position. The body is drawn and translated
// into its final position.
//----------------------------------------------------------------------
void drawSnowman(){
	glColor3f(1, 1, 0);

	glPushMatrix();
		glPushMatrix();
			glScalef(3, 3, 3);
			glRotatef(180, 1, 0, 0);
			glRotatef(90, 0, 1, 0);
			glRotatef(90, 1, 0, 0);
			glTranslatef(0, -0.3, 0);
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(0, 0, 0);
			glVertex3f(0.5, 0, -0.2);
			glVertex3f(0.4, 0.3, -0.1);
			glVertex3f(0.2, 0.5, -0.2);
			glVertex3f(0, 0.5, 0);
			glVertex3f(-0.2, 0.5, -0.2);
			glVertex3f(-0.4, 0.3, -0.1);
			glVertex3f(-0.5, 0, -0.2);
			glEnd();
		glPopMatrix();
		glColor3f(1, 0, 0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 0.4);	
		for (int i = 0; i < 6; i++)
		{
			if (i != 0){
				glRotatef(15, 1, 0, 0);
				glTranslatef(0, 0, 0.5);
			}
			GLUquadricObj *podstawa = gluNewQuadric();
			gluCylinder(podstawa, 1.0, 0.8, 1, 40, 40);

		}
		glTranslatef(0, 0, 1);
		glutSolidCone(0.9, 2, 40, 40);

		glRotatef(-70, 0, 0, 1);
		glTranslatef(-0.4, 0, 1);
		glColor3f(1, 1, 1); 	
		glPushMatrix();
			glTranslatef(0, 0.3, 0);
			glutSolidSphere(0.3, 30, 30);
			glTranslatef(0, 0, 0.23);
			glColor3f(0, 0, 0);
			glutSolidSphere(0.08, 12, 12);
		glPopMatrix();
		glColor3f(1, 1, 1);
		glPushMatrix();
			glTranslatef(0, -0.3, 0);
			glutSolidSphere(0.3, 30, 30);
			glTranslatef(0, 0, 0.23);
			glColor3f(0, 0, 0);
			glutSolidSphere(0.08, 12, 12);
		glPopMatrix();
	glPopMatrix();
	

}

//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and requests that the scene be
// redrawn.
//----------------------------------------------------------------------
void update(void)
{
	if (deltaMove) { // update camera position
		x += deltaMove * lx * 0.1;
		y += deltaMove * ly * 0.1;
	}
	glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene(void)
{


	int i, j;

	// Clear color and depth buffers
	glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Set the camera centered at (x,y,1) and looking along directional
	// vector (lx, ly, 0), with the z-axis pointing up
	gluLookAt(
		x, y, 4.0,
		x + lx, y + ly, 1.0,
		0.0, 0.0, 1.0);

	glColor3f(0.9f,0.9f,0.9f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(-100.0, -100.0, 0.0);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(-100.0, 100.0, 0.0);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(100.0, 100.0, 0.0);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(100.0, -100.0, 0.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);


	// Draw 36 snow men
	for (i = -3; i < 3; i++)
	for (j = -3; j < 3; j++) {
		glPushMatrix();
		glTranslatef(i*7.5, j*7.5, 0);
		drawSnowman();
		glPopMatrix();
	}

	glutSwapBuffers(); // Make it all visible
}

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// pressSpecialKey: Up arrow = forward motion, down arrow = backwards
// releaseSpecialKey: Set incremental motion to zero
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') exit(0);
}

void pressSpecialKey(int key, int xx, int yy)
{
	switch (key) {
	case GLUT_KEY_UP: deltaMove = 1.0; break;
	case GLUT_KEY_DOWN: deltaMove = -1.0; break;
	}
}

void releaseSpecialKey(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: deltaMove = 0.0; break;
	case GLUT_KEY_DOWN: deltaMove = 0.0; break;
	}
}

//----------------------------------------------------------------------
// Process mouse drag events
// 
// This is called when dragging motion occurs. The variable
// angle stores the camera angle at the instance when dragging
// started, and deltaAngle is a additional angle based on the
// mouse movement since dragging started.
//----------------------------------------------------------------------
void mouseMove(int x, int y)
{
	if (isDragging) { // only when dragging
		// update the change in angle
		deltaAngle = (x - xDragStart) * 0.005;

		// camera's direction is set to angle + deltaAngle
		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) { // left mouse button pressed
			isDragging = 1; // start dragging
			xDragStart = x; // save x where button first pressed
		}
		else  { /* (state = GLUT_UP) */
			angle += deltaAngle; // update camera turning angle
			isDragging = 0; // no longer dragging
		}
	}
}

//----------------------------------------------------------------------
// Main program  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
	printf("\n\
		   -----------------------------------------------------------------------\n\
		     OpenGL Sample Program:\n\
			   - Drag mouse left-right to rotate camera\n\
			     - Hold up-arrow/down-arrow to move camera forward/backward\n\
				   - q or ESC to quit\n\
				   -----------------------------------------------------------------------\n");

	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 400);
	glutCreateWindow("OpenGL/GLUT Sampe Program");

	// register callbacks
	glutReshapeFunc(changeSize); // window reshape callback
	glutDisplayFunc(renderScene); // (re)display callback
	glutIdleFunc(update); // incremental update 
	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion
	glutKeyboardFunc(processNormalKeys); // process standard key clicks
	glutSpecialFunc(pressSpecialKey); // process special key pressed
	// Warning: Nonstandard function! Delete if desired.
	glutSpecialUpFunc(releaseSpecialKey); // process special key release

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	texture = LoadBMP("grill.bmp");

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0; // this is just to keep the compiler happy
}