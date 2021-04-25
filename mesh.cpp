#include "mesh.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif

#include <iostream>
#include <random>

Mesh::Mesh(int dimX, int dimZ) : worldDimX(dimX), worldDimZ(dimZ) {
    // Init heightmap
    hmap = new float[worldDimX * worldDimZ];
}

Mesh::~Mesh() {
    delete[] hmap;
    delete[] verts;
    delete[] indicies;
}

void Mesh::generateHeightMap() {
    /* Generate terrain */
    std::srand(std::time(nullptr));
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(2.0,0.25);

    for (int z = 0; z < worldDimZ; z++) {
        for (int x = 0; x < worldDimX; x++) {
            hmap[(z * worldDimX) + x] = distribution(generator);
        }
    }
}

void Mesh::generateMesh() {
    // Procedural generation
    generateHeightMap();

    /* Calculate plane verticies + indicies */
    // 3 coordinates for every vertex/point in tilemap
    vertsSize = 3 * (worldDimX + 1) * (worldDimZ + 1);
    verts = new float[vertsSize * sizeof(float)];

    // 2 triangles * 3 verts/tri * num tiles
    indiciesSize = 2 * 3 * worldDimX * worldDimZ;
    indicies = new GLushort[indiciesSize * sizeof(GLushort)];

    // Calculate verticies (+ 1 for tiles -> verticies)
    std::cout << worldDimX << ", " << worldDimZ << std::endl;
    int w = 0;
    for (int z = 0; z < (worldDimZ + 1); z++) {
        for (int x = 0; x < (worldDimX + 1); x++) {
            
            // 3 coordinates per vertex
            int base = 3 * ((z * (worldDimX + 1)) + x);
            verts[base + 0] = x;
            verts[base + 1] = hmap[(z * worldDimX) + x];
            // verts[base + 1] = 0.0;
            verts[base + 2] = z;
        }
    }

    // Calculate indicies (2 tris per tile)
    // std::cout << "Indicies:" << std::endl;
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

}