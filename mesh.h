#ifndef MESH_H
#define MESH_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif

/**
 * @brief Procedural Terrain Mesh
 */
class Mesh {
    public:
        /* Mesh dimensions */
        const int worldDimX, worldDimZ;

        /* Geometry variables */
        float *verts;
        GLushort *indicies;
        int vertsSize;
        int indiciesSize;

        Mesh(int dimX, int dimZ);
        ~Mesh();
        void generateMesh();
    
    private:
        // Heightmap
        float *hmap;
        
        void generateHeightMap();

};

#endif