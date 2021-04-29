/**
*	@author Pranav Nair
*   CS 334 Final Project
*	Procedural Terrain Generation
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
// #include <random>


#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include "glui/include/GL/glui.h"
#else
#include "GL/glut.h"
#endif

#include "mesh.h"

/* Window Variables */
int windowWidth = 800;
int windowHeight = 	800;

/* Camera Variables */
float fovy = 60.0;
float aspect = windowWidth / windowHeight;
float zNear = 1.0;
float zFar = 100.0;
GLfloat cameraX = 0, cameraZ = -7, cameraY = 0; // Starting camera coordinates
float cameraVX = 0.25, cameraVZ = 0.25, cameraVY = 0.25;
float cameraRotateX = 0, cameraRotateY = 0;
GLfloat cameraRotateV = 10.0;

GLfloat myModelMat[4][4] = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, -1, 0, 1 }
};

/* Terrain Mesh */
Mesh *terrain_mesh;
Mesh *water_mesh;

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
		// std::cout << "Moving +z" << std::endl;
		// cameraZ++;
		cameraZ += forwardZ;
		cameraX += forwardX;

	} else if (key == GLUT_KEY_DOWN) {
		// std::cout << "Moving -z" << std::endl;
		// cameraZ--;
		cameraZ -= forwardZ;
		cameraX -= forwardX;

	} else if (key == GLUT_KEY_RIGHT) {
		// std::cout << "Moving -x" << std::endl;
		// cameraX--;
		cameraZ += rightZ;
		cameraX += rightX;

	} else if (key == GLUT_KEY_LEFT) {
		// std::cout << "Moving +x" << std::endl;
		// cameraX++;
		cameraZ -= rightZ;
		cameraX -= rightX;

	}

	// std::cout << "Position: x: " << cameraX << ", z: " << cameraZ << std::endl;
	// std::cout << "Forward: x: " << forwardX << ", z: " << forwardZ << std::endl;
}

/**
*    #### Function invoked when an event on regular keys occur
*/
void keyboard(unsigned char k, int x, int y)
{
	/* Show which key was pressed */
	// std::cout << "Pressed \"" << k << "\" ASCII: " << (int)k << std::endl;

	if (k == 'd') {
		// std::cout << "Rotating +y" << std::endl;
		cameraRotateY++;
	} else if (k == 'a') {
		// std::cout << "Rotating -y" << std::endl;
		cameraRotateY--;
	} else if (k == 's') {
		// std::cout << "Rotating +x" << std::endl;
		cameraRotateX++;
	} else if (k == 'w') {
		// std::cout << "Rotating -x" << std::endl;
		cameraRotateX--;
	} else if (k == 27) {
		/* Close application if ESC is pressed */
		exit(0);
	} else if (k == 'z') {
		// std::cout << "Moving +y" << std::endl;
		cameraY--;
	} else if (k == 'x') {
		// std::cout << "Moving -y" << std::endl;
		cameraY++;
	}

	// std::cout << "RotationY: " << cameraRotateY << std::endl;
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
	glVertexPointer(3, GL_FLOAT, 0, terrain_mesh->verts);
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glColor3f(0.92, 0.71, 0.20);
	glDrawElements(GL_TRIANGLES, terrain_mesh->indiciesSize, GL_UNSIGNED_SHORT, terrain_mesh->indicies);
	glColor3f(0.0, 0.0, 0.0);
	// DO NOT USE GL_LINE_STRIP: Causes weird random lines
	glDrawElements(GL_LINES, terrain_mesh->indiciesSize, GL_UNSIGNED_SHORT, terrain_mesh->indicies);

	glVertexPointer(3, GL_FLOAT, 0, water_mesh->verts);
	glTranslatef(0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glDrawElements(GL_TRIANGLES, water_mesh->indiciesSize, GL_UNSIGNED_SHORT, water_mesh->indicies);
	glColor3f(0.0, 0.0, 0.0);
	// DO NOT USE GL_LINE_STRIP: Causes weird random lines
	glDrawElements(GL_LINES, water_mesh->indiciesSize, GL_UNSIGNED_SHORT, terrain_mesh->indicies);

	glPopMatrix();
	
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

	// Generate Meshes
	// TEMP: rand() does not prodice a 32 bit random number
	srand(time(NULL));
	terrain_mesh = new Mesh(50, 50, rand(), 15.0, 0.0, 2.0, 1);
	terrain_mesh->generateMesh();

	water_mesh = new Mesh(50, 50, rand(), 0.0, -1.0, 0.0, 1);
	water_mesh->generateMesh();

	std::cout << terrain_mesh->vertsSize << std::endl;
	std::cout << terrain_mesh->indiciesSize << std::endl;
	
	// std::cout << "Indicies:" << std::endl;
	// for (int i = 0; i < terrain_mesh->indiciesSize; i++)
	// {
	// 	std::cout << (int)(terrain_mesh->indicies[i]) << " ";
	// 	if ((i+1) % 3 == 0) std::cout << std::endl;
	// 	if ((i+1) % 6 == 0) std::cout << std::endl;
	// }

	// std::cout << "Verticies:" << std::endl;
	// for (int i = 0; i < terrain_mesh->vertsSize; i++)
	// {
	// 	std::cout << (float)(terrain_mesh->verts[i]) << " ";
	// 	if ((i+1) % 3 == 0) std::cout << std::endl;
	// 	if ((i+1) % 9 == 0) std::cout << std::endl;
	// }

	/* Start the main GLUT loop */
    glutMainLoop();

	//TODO: destroy mallocs
}