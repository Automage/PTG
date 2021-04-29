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
        float r, g, b;

        Mesh(int dimX, int dimZ, uint32_t seed, float maxh, float offset, float freq, int octaves);
        ~Mesh();
        void generateMesh();
        void encourageMountain(int c_x, int c_z, float height, float width);
        void encourageLake(int c_x, int c_z, float width);
        void encourageRiver(int x0, int z0, int x1, int z1);
    
    private:
        // Mesh specifiers
        int perlin_octaves;
        float perlin_freq, max_height, offset;

        // Heightmap
        float *hmap;
        uint32_t seed;
        
        void generateGaussianHeightMap();
        void generateHeightMap();

};

#endif