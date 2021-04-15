/**
*	@author Pranav Nair
*   CS 334 Final Project
*	Procedural Terrain Generation
*/

#include <iostream>

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
GLfloat cameraX = 0, cameraZ = -10, cameraY = 0; // Starting camera coordinates
float cameraVX = 1, cameraVZ = 1, cameraVY = 1;
float cameraRotateX = 0, cameraRotateY = 0;
GLfloat cameraRotateV = 5.0;

GLfloat myModelMat[4][4] = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, -1, 0, 1 }
};

/* Geometry variables */
float triangleVertices[9] = {
     1.0,  0.0,  1.0,        // (1,1)
     0.0,  0.0,  1.0,        // (0,1)
     0.0,  0.0,  0.0,        // (0,0)
};

GLubyte triangleIndices[6] = {
    0, 1, 2,	// Triangles for the top face
};

/* Terrain variables */
int world[4][4] = 
{
	{1, 2, 3, 4},
	{0, 1, 2, 3},
	{0, 0, 1, 2},
	{0, 0, 0, 1},
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
	if (key == GLUT_KEY_UP) {
		std::cout << "Moving +z" << std::endl;
		cameraZ++;
	} else if (key == GLUT_KEY_DOWN) {
		std::cout << "Moving -z" << std::endl;
		cameraZ--;
	} else if (key == GLUT_KEY_RIGHT) {
		std::cout << "Moving -x" << std::endl;
		cameraX--;
	} else if (key == GLUT_KEY_LEFT) {
		std::cout << "Moving +x" << std::endl;
		cameraX++;
	} else if (key == GLUT_KEY_F1) {
		std::cout << "Moving +y" << std::endl;
		cameraY--;
	} else if (key == GLUT_KEY_F2) {
		std::cout << "Moving -y" << std::endl;
		cameraY++;
	}
}

/**
*    #### Function invoked when an event on regular keys occur
*/
void keyboard(unsigned char k, int x, int y)
{
	/* Show which key was pressed */
	std::cout << "Pressed \"" << k << "\" ASCII: " << (int)k << std::endl;

	if (k == 'a') {
		std::cout << "Rotating +y" << std::endl;
		cameraRotateY++;
	} else if (k == 'd') {
		std::cout << "Rotating -y" << std::endl;
		cameraRotateY--;
	} else if (k == 'w') {
		std::cout << "Rotating +x" << std::endl;
		cameraRotateX++;
	} else if (k == 's') {
		std::cout << "Rotating -x" << std::endl;
		cameraRotateX--;
	} else if (k == 27) {
		/* Close application if ESC is pressed */
		exit(0);
	}
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
			float triangleVertices2[9] = {
				0.0,  world[i][j],  		0.0,        // (0,0)
				0.0,  world[i + 1][j],  	1.0,        // (1,1)
				1.0,  world[i + 1][j + 1],  1.0,        // (0,1)
			};
			// float triangleVertices2[9] = {
			// 	1.0,  5.0,  1.0,        // (1,1)
			// 	0.0,  4.0,  1.0,        // (0,1)
			// 	0.0,  3.0,  0.0,        // (0,0)
			// };
			glVertexPointer(3, GL_FLOAT, 0, triangleVertices2);
			glPushMatrix();
			glTranslatef(i, 0.0, j);
			glColor3f(1.0, 0.0, 0.0);
			glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_BYTE, triangleIndices);
			glColor3f(0.0, 0.0, 0.0);
			glDrawElements(GL_LINE_STRIP, 9, GL_UNSIGNED_BYTE, triangleIndices);
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