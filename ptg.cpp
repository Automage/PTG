/**
*	@author Pranav Nair
*   CS 334 Final Project
*	Procedural Terrain Generation
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include "glui/include/GL/glui.h"
#else
#include "GL/glut.h"
#endif

/* Window Variables */
int windowWidth = 800;
int windowHeight = 	800;

/* Camera Variables */
float fovy = 45.0;
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

/* Geometry variables */
// float tileVertices[12] = {
//      0.0,  0.0,  0.0,        // (0,0)
//      0.0,  0.0,  1.0,        // (0,1)
// 	 1.0,  0.0,  1.0,        // (1,1)
// 	 1.0,  0.0,  0.0,        // (0,1)
// };

// GLubyte tileIndices[6] = {
//     0, 1, 2,	// Triangles for the top face
// 	0, 3, 2,	// Triangles for the top face
// };
float *meshVerticies;
GLushort *meshIndicies;
int meshVerticiesSize;
int meshIndiciesSize;

/* Terrain variables */
const int worldDimX = 16, worldDimZ = 16;
int world[worldDimX][worldDimZ];

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
	std::cout << "Pressed \"" << k << "\" ASCII: " << (int)k << std::endl;

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
	// for(int i = 0; i < worldDimX - 1; i += 1) {
	// 	for(int j = 0; j < worldDimZ - 1; j += 1) {
	// 		// Calculate tile verticies (2 triangles which may not be on the same plane)
	// 		float tileVertices2[12] = {
	// 			0.0,  world[i][j],  0.0,        // (0,0)
	// 			0.0,  world[i][j + 1],  1.0,        // (0,1)
	// 			1.0,  world[i + 1][j + 1],  1.0,        // (1,1)
	// 			1.0,  world[i + 1][j],  0.0,        // (0,1)
	// 		};

	// 		glVertexPointer(3, GL_FLOAT, 0, tileVertices2);
	// 		glPushMatrix();
	// 		glTranslatef(i, 0.0, j);
	// 		glColor3f(0.0, 0.0, 1.0);
	// 		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, tileIndices);
	// 		glColor3f(0.0, 0.0, 0.0);
	// 		glDrawElements(GL_LINE_STRIP, 12, GL_UNSIGNED_BYTE, tileIndices);
	// 		glPopMatrix();
	// 	}
	// }

	glVertexPointer(3, GL_FLOAT, 0, meshVerticies);
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glDrawElements(GL_TRIANGLES, meshIndiciesSize, GL_UNSIGNED_SHORT, meshIndicies);
	glColor3f(1.0, 0.0, 0.0);
	glDrawElements(GL_LINE_STRIP, meshIndiciesSize, GL_UNSIGNED_SHORT, meshIndicies);
	glPopMatrix();
	
	/* Disable client */
	glDisableClientState(GL_VERTEX_ARRAY);

    /* Force execution of OpenGL commands */
    glFlush();

    /* Swap buffers for animation */
    glutSwapBuffers();
}

void generateMesh() {
	/* Generate terrain */
	std::srand(std::time(nullptr));
	std::default_random_engine generator;
  	std::normal_distribution<float> distribution(5.0,2.0);

	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++) {
			// world[i][j] = std::rand() % 6;
			world[i][j] = distribution(generator);
			// std::cout << world[i][j] << std::endl;
		}
	}

	/* Calculate plane verticies + indicies */
	// 3 coordinates for every vertex/point in tilemap
	meshVerticiesSize = 3 * (worldDimX + 1) * (worldDimZ + 1);
	float *verts = new float[meshVerticiesSize * sizeof(float)];

	// 2 triangles * 3 verts/tri * num tiles
	meshIndiciesSize = 2 * 3 * worldDimX * worldDimZ;
	GLushort *indicies = new GLushort[meshIndiciesSize * sizeof(GLushort)];

	// Calculate verticies (+ 1 for tiles -> verticies)
	std::cout << worldDimX << ", " << worldDimZ << std::endl;
	int w = 0;
	for (int z = 0; z < (worldDimZ + 1); z++) {
		for (int x = 0; x < (worldDimX + 1); x++) {
			
			// 3 coordinates per vertex
			int base = 3 * ((z * (worldDimX + 1)) + x);
			verts[base + 0] = x;
			// verts[base + 1] = (float) world[x][z];
			verts[base + 1] = 0.0;
			verts[base + 2] = z;
		}
	}

	// Calculate indicies (2 tris per tile)
	std::cout << "Indicies:" << std::endl;
	for (int z = 0; z < worldDimZ; z++) {
		for (int x = 0; x < worldDimX; x++) {
			/*
			 * |2 /|
			 * | / |
			 * |/ 1|
			 */

			// 6 indicies stored per tile
			int base = 2 * 3 * ((z * worldDimX) + x);

			// (worldDimX + 1) for tiles -> verticies
			// Triangle 1 (0,0), (0,1), (1,1)
			indicies[base + 0] = (z * (worldDimX + 1)) + x;
			indicies[base + 1] = (z * (worldDimX + 1)) + (x + 1);
			indicies[base + 2] = ((z + 1) * (worldDimX + 1)) + (x + 1);
			
			// Triangle 2 (0,0), (1,0), (1,1)
			indicies[base + 3] = (z * (worldDimX + 1)) + x;
			indicies[base + 4] = ((z + 1) * (worldDimX + 1)) + x;
			indicies[base + 5] = ((z + 1) * (worldDimX + 1)) + (x + 1);
			std::cout << "(" << x << ", " << z << "):" << base << "\t(" << (int)indicies[base] << ", " << (int)indicies[base + 1] << ", " << (int)indicies[base + 2] << ") (" << (int)indicies[base + 3] << ", " << (int)indicies[base + 4] << ", " << (int)indicies[base + 5] << ")" << std::endl;
		}
	}

	meshVerticies = verts;
	meshIndicies = indicies;
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

	// Generate mesh + calculate plane verticies 
	generateMesh();

	std::cout << meshVerticiesSize << std::endl;
	std::cout << meshIndiciesSize << std::endl;
	// std::cout << "Indicies:" << std::endl;
	// for (int i = 0; i < meshIndiciesSize; i++)
	// {
	// 	std::cout << (int)(meshIndicies[i]) << " ";
	// 	if ((i+1) % 3 == 0) std::cout << std::endl;
	// 	if ((i+1) % 6 == 0) std::cout << std::endl;
	// }

	// std::cout << "Verticies:" << std::endl;
	// for (int i = 0; i < meshVerticiesSize; i++)
	// {
	// 	std::cout << (float)(meshVerticies[i]) << " ";
	// 	if ((i+1) % 3 == 0) std::cout << std::endl;
	// 	if ((i+1) % 9 == 0) std::cout << std::endl;
	// }

	/* Start the main GLUT loop */
    glutMainLoop();

	//TODO: destroy mallocs
}