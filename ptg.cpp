/**
*	@author Pranav Nair
*   CS 334 Final Project
*	Procedural Terrain Generation
*/

#include <iostream>
#include <cmath>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include "glui/include/GL/glui.h"
#else
#include "GL/glut.h"
#endif

/* Window Variables */
int windowWidth = 800;
int windowHeight = 600;

/* Camera Variables */
float fovy = 45.0;
float aspect = windowWidth / windowHeight;
float zNear = 1.0;
float zFar = 100.0;
GLfloat cameraX = 0, cameraZ = -7, cameraY = 0; // Starting camera coordinates
float cameraVX = 1, cameraVZ = 1, cameraVY = 1;
float cameraRotateX = 0, cameraRotateY = 0;
GLfloat cameraRotateV = 10.0;

GLfloat myModelMat[4][4] = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, -1, 0, 1 }
};

/* Geometry variables */
float tileVertices[12] = {
     0.0,  0.0,  0.0,        // (0,0)
     0.0,  0.0,  1.0,        // (0,1)
	 1.0,  0.0,  1.0,        // (1,1)
	 1.0,  0.0,  0.0,        // (0,1)
};

GLubyte tileIndices[6] = {
    0, 1, 2,	// Triangles for the top face
	0, 3, 2,	// Triangles for the top face
};

/* Terrain variables */
int world[4][4] = 
{
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 0, 0},
};

void openGLInit() {
	/* Set clear color */
	glClearColor(1.0, 1.0, 1.0, 0.0);

	/* Enable the depth buffer */
	glEnable(GL_DEPTH_TEST);

	printf("%s\n", glGetString(GL_VERSION));
}

/**
*    Function invoked when window system events are not being received
*/
void idle()
{
    /* Redraw the window */
    glutPostRedisplay();
}

/**
*	#### Function invoked when an event on special keys occur
*/
void special(int key, int x, int y)
{
	// Rotate by counterclockwise 90 offset
	float forwardX = -sinf(M_PI * (cameraRotateV * (cameraRotateY)) / 180.0);
	float forwardZ = cosf(M_PI * (cameraRotateV * (cameraRotateY)) / 180.0);
	float magnitude = sqrtf(powf(forwardX, 2.0) + powf(forwardZ, 2.0));
	forwardX /= magnitude;
	forwardZ /= magnitude;
	float rightX = -forwardZ;
	float rightZ = forwardX;

	if (key == GLUT_KEY_UP) {
		std::cout << "Moving +z" << std::endl;
		// cameraZ++;
		cameraZ += forwardZ;
		cameraX += forwardX;

	} else if (key == GLUT_KEY_DOWN) {
		std::cout << "Moving -z" << std::endl;
		// cameraZ--;
		cameraZ -= forwardZ;
		cameraX -= forwardX;

	} else if (key == GLUT_KEY_RIGHT) {
		std::cout << "Moving -x" << std::endl;
		// cameraX--;
		cameraZ += rightZ;
		cameraX += rightX;

	} else if (key == GLUT_KEY_LEFT) {
		std::cout << "Moving +x" << std::endl;
		// cameraX++;
		cameraZ -= rightZ;
		cameraX -= rightX;

	}

	std::cout << "Position: x: " << cameraX << ", z: " << cameraZ << std::endl;
	std::cout << "Forward: x: " << forwardX << ", z: " << forwardZ << std::endl;
}

/**
*    #### Function invoked when an event on regular keys occur
*/
void keyboard(unsigned char k, int x, int y)
{
	/* Show which key was pressed */
	std::cout << "Pressed \"" << k << "\" ASCII: " << (int)k << std::endl;

	if (k == 'd') {
		std::cout << "Rotating +y" << std::endl;
		cameraRotateY++;
	} else if (k == 'a') {
		std::cout << "Rotating -y" << std::endl;
		cameraRotateY--;
	} else if (k == 's') {
		std::cout << "Rotating +x" << std::endl;
		cameraRotateX++;
	} else if (k == 'w') {
		std::cout << "Rotating -x" << std::endl;
		cameraRotateX--;
	} else if (k == 27) {
		/* Close application if ESC is pressed */
		exit(0);
	} else if (k == 'z') {
		std::cout << "Moving +y" << std::endl;
		cameraY--;
	} else if (k == 'x') {
		std::cout << "Moving -y" << std::endl;
		cameraY++;
	}

	std::cout << "RotationY: " << cameraRotateY << std::endl;
}

/**
*    Function invoked for drawing using OpenGL
*/
void display()
{
	static int frameCount=0;

	/* #### frame count, might come in handy for animations */
	frameCount++;
	
    /* Clear the window */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* Set the perspective projection */
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);

    /* #### Load/set the model view matrix */
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf((GLfloat *)myModelMat);

	// Translate to camera coordinates
	glTranslatef(0, 0, 0);

	// Rotate camera to current rotation
	glRotatef(cameraRotateV * cameraRotateX, 1.0, 0, 0);
	glRotatef(cameraRotateV * cameraRotateY, 0, 1.0, 0);

	// Translate to camera coordinates
	glTranslatef(cameraX * cameraVX, cameraY * cameraVY, cameraZ * cameraVZ);

    /* Enable client */
	glEnableClientState(GL_VERTEX_ARRAY);
	
	/* Draw the world */
	for(int i = 0; i < 3; i += 1) {
		for(int j = 0; j < 3; j += 1) {
			// Calculate tile verticies (2 triangles which may not be on the same plane)
			float tileVertices2[12] = {
				0.0,  world[i][j],  0.0,        // (0,0)
				0.0,  world[i][j + 1],  1.0,        // (0,1)
				1.0,  world[i + 1][j + 1],  1.0,        // (1,1)
				1.0,  world[i + 1][j],  0.0,        // (0,1)
			};

			glVertexPointer(3, GL_FLOAT, 0, tileVertices2);
			glPushMatrix();
			glTranslatef(i, 0.0, j);
			glColor3f(1.0, 0.0, 0.0);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, tileIndices);
			glColor3f(0.0, 0.0, 0.0);
			glDrawElements(GL_LINE_STRIP, 12, GL_UNSIGNED_BYTE, tileIndices);
			glPopMatrix();
		}
	}
	
	/* Disable client */
	glDisableClientState(GL_VERTEX_ARRAY);

    /* Force execution of OpenGL commands */
    glFlush();

    /* Swap buffers for animation */
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    /* Initialize the GLUT window */
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(30, 30);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Procedural Terrain Generation");

	/* Set OpenGL initial state */
	openGLInit();

	/* Callback functions */
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	/* Start the main GLUT loop */
    glutMainLoop();
}