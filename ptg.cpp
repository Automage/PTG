/**
*	@author Pranav Nair
*   CS 334 Final Project
*	Procedural Terrain Generation
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>

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
float cameraVX = 0.4, cameraVZ = 0.4, cameraVY = 0.4;
float cameraRotateX = 0, cameraRotateY = 0;
GLfloat cameraRotateV = 10.0;

GLfloat myModelMat[4][4] = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, -1, 0, 1 }
};

/* Mesh variables*/
bool generateWater = false;
Mesh *terrain_mesh;
Mesh *water_mesh;

/* Color constants */
#define DEFAULT_TERRAIN_R 0.92
#define DEFAULT_TERRAIN_G 0.71
#define DEFAULT_TERRAIN_B 0.20
#define DEFAULT_WATER_R 0.0
#define DEFAULT_WATER_G 0.0
#define DEFAULT_WATER_B 1.0

/**
 * @brief Generate meshes from grammar file
 * 
 * @param filename grammar file
 * @return int return code (0 for success)
 */
int generateFromGrammar(std::string filename) {
	// Mesh variables
	bool dimensionCheck, terrainCheck;
	int dimX, dimZ;
	float height, density, water_level;
	int gen_water, roughness;

	srand(time(NULL));

	// Read grammar file
	std::ifstream infile(filename);
	std::string line;
	
	while (std::getline(infile, line)) {
		// Ignore comments
		if (line[0] == '#' || line.empty())
			continue;

		std::istringstream iss(line);

		// Read and set properties
		if (!dimensionCheck) {
			// Check terrain dimensions first
			iss >> dimX >> dimZ;
			std::cout << "dimx: " << dimX << ", dimZ: " << dimZ << std::endl; 
			dimensionCheck = true;

		} else if (!terrainCheck) {
			// Obtain terrain characteristics second
			iss >> height >> roughness >> density >> gen_water >> water_level;
			std::cout << "height: " << height << ", roughness: " << roughness << ", mountain density: " << density 
					  << ", generate water: " << (gen_water == 1 ? "yes" : "no") << ", water level: " << water_level << std::endl; 
			
			// Create meshes
			// TEMP: rand() does not prodice a 32 bit random number
			terrain_mesh = new Mesh(dimX, dimZ, rand(), height, 0.0, density, roughness);
			terrain_mesh->r = DEFAULT_TERRAIN_R;
			terrain_mesh->g = DEFAULT_TERRAIN_G;
			terrain_mesh->b = DEFAULT_TERRAIN_B;
			
			if (gen_water == 1) {
				generateWater = true;
				water_mesh = new Mesh(dimX, dimZ, rand(), 0.0, water_level, 0.0, 1);
				water_mesh->r = DEFAULT_WATER_R;
				water_mesh->g = DEFAULT_WATER_G;
				water_mesh->b = DEFAULT_WATER_B;
			}

			terrainCheck = true;

		} else {
			// Encouragers (Encourage certain features in terrain)
			std::string token;
			iss >> token;

			if (token == "M") {
				// Encourage mountain
				int x, z;
				float height, width;

				iss >> x >> z >> height >> width;
				terrain_mesh->encourageMountain(x, z, height, width);

				std::cout << "(Mountain) Location: (" << x << ", " << z << "), Height: " 
						  << height << ", Width: " << width << std::endl;

			} else if (token == "L") {
				// Encourage lake
				int x, z;
				float width;

				iss >> x >> z >> width;
				terrain_mesh->encourageLake(x, z, width);

				std::cout << "(Lake) Location: (" << x << ", " << z << "), Width: " << width << std::endl;

			} else if (token == "R") {
				// Encourage river
				int x0, z0, x1, z1;
				// float width;

				iss >> x0 >> z0 >> x1 >> z1;
				terrain_mesh->encourageRiver(x0, z0, x1, z1);

				std::cout << "(River) Start point: (" << x0 << ", " << z0 << "), End point: (" 
						  << x1 << ", " << z1 << ")" << std::endl;

			} else if (token == "C") {
				// Custom terrain color
				if (generateWater) {
					iss >> terrain_mesh->r >> terrain_mesh->g >> terrain_mesh->b 
					    >> water_mesh->r >> water_mesh->g >> water_mesh->b; 
					std::cout << "(Color) Terrain: (" << terrain_mesh->r << ", " << terrain_mesh->g 
							  << ", " << terrain_mesh->b << ") Water: (" << water_mesh->r << ", " 
							  << water_mesh->g << ", " << water_mesh->b << ")" << std::endl;
				} else {
					iss >> terrain_mesh->r >> terrain_mesh->g >> terrain_mesh->b; 
					std::cout << "(Color) Terrain: (" << terrain_mesh->r << ", " << terrain_mesh->g 
							  << ", " << terrain_mesh->b << ")" << std::endl;
				}
				
			} else {
				// Unknown syntax
				// std::cout << "ERROR: Unknown grammar: "<< token << std::endl;
				// return 1;
			}
		}

	}

	// Generate Meshes
	terrain_mesh->generateMesh();
	if (generateWater)
		water_mesh->generateMesh();

	return 0;
}

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
	
	/* Draw Terrain */
	glVertexPointer(3, GL_FLOAT, 0, terrain_mesh->verts);
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glColor3f(terrain_mesh->r, terrain_mesh->g, terrain_mesh->b);
	glDrawElements(GL_TRIANGLES, terrain_mesh->indiciesSize, GL_UNSIGNED_SHORT, terrain_mesh->indicies);
	glColor3f(0.0, 0.0, 0.0);
	// DO NOT USE GL_LINE_STRIP: Causes weird random lines
	glDrawElements(GL_LINES, terrain_mesh->indiciesSize, GL_UNSIGNED_SHORT, terrain_mesh->indicies);

	/* Draw Water */
	if (generateWater) {
		glVertexPointer(3, GL_FLOAT, 0, water_mesh->verts);
		glTranslatef(0.0, 0.0, 0.0);
		glColor3f(water_mesh->r, water_mesh->g, water_mesh->b);
		glDrawElements(GL_TRIANGLES, water_mesh->indiciesSize, GL_UNSIGNED_SHORT, water_mesh->indicies);
		glColor3f(0.0, 0.0, 0.0);
		// DO NOT USE GL_LINE_STRIP: Causes weird random lines
		glDrawElements(GL_LINES, water_mesh->indiciesSize, GL_UNSIGNED_SHORT, terrain_mesh->indicies);

		glPopMatrix();
	}
	/* Disable client */
	glDisableClientState(GL_VERTEX_ARRAY);

    /* Force execution of OpenGL commands */
    glFlush();

    /* Swap buffers for animation */
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    // Check for grammar file
	if (argc != 2) {
		std::cout << "Please provide grammar file..." << std::endl;
		exit(1);
	}

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
	std::string grammar_file(argv[1]);
	if (generateFromGrammar(grammar_file) != 0) {
		exit(1);
	}

	std::cout << std::endl << "Verts: " << terrain_mesh->vertsSize << std::endl;
	
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