#include "mesh.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif

#include <iostream>
#include <random>
#include <cmath>
#include <utility>

#include "PerlinNoise/PerlinNoise.hpp"

/* Helper functions */

// 2D Gaussian function
float gaussian2D(int x, int z, int c_x, int c_z, float height, float var_x, float var_z) {
    float exponent = -(powf(x - c_x, 2.0)/var_x + powf(z - c_z, 2.0)/var_z);
    return height * expf(exponent);
}

// Dot product
float dot(std::pair<float, float> v, std::pair<float, float> w) {
    return (v.first * w.first) + (v.second * w.second);
}

// Distance between two points
float dist(std::pair<float, float> v, std::pair<float, float> w) {
    std::pair<float, float> diff(v.first - w.first, v.second - w.second);
    return sqrtf(dot(diff, diff));
}

// Returns distance between point and line segment
// p - point, s0 - segment point 0, s1 - segment point 1
// Referenced from https://geomalgorithms.com/a02-_lines.html
float distanceFromSegment(int p_x, int p_z, int s0_x, int s0_z, int s1_x, int s1_z) {
    std::pair<float, float> p(p_x, p_z);
    std::pair<float, float> s0(s0_x, s0_z);
    std::pair<float, float> s1(s1_x, s1_z);

    std::pair<float, float> v(s1.first - s0.first, s1.second - s0.second);
    std::pair<float, float> w(p.first - s0.first, p.first - s0.first);

    float c1 = dot(v, w);
    if (c1 <= 0)
        return dist(p, s0);

    float c2 = dot(v, v);
    if (c2 <= c1)
        return dist(p, s1);

    float b = c1 / c2;
    std::pair<float, float> pb(s0.first + (b * v.first), s0.second + (b * v.second));
    return dist(p, pb);
}

/* Mesh methods */

Mesh::Mesh(int dimX, int dimZ, uint32_t s, float maxh, float offset, float freq, int octaves) 
          : worldDimX(dimX), worldDimZ(dimZ), seed(s), max_height(maxh), offset(offset), 
            perlin_freq(freq), perlin_octaves(octaves) {
    // Init heightmap
    // () initializes to 0
    hmap = new float[worldDimX * worldDimZ]();
}

Mesh::~Mesh() {
    delete[] hmap;
    delete[] verts;
    delete[] indicies;
}

void Mesh::generateGaussianHeightMap() {
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

void Mesh::generateHeightMap() {
    /* Generate terrain */
    siv::BasicPerlinNoise<float> perlin(seed);

    float fx = worldDimX / perlin_freq;
    float fz = worldDimZ / perlin_freq;

    for (std::int32_t z = 0; z < worldDimZ; z++) {
        for (std::int32_t x = 0; x < worldDimX; x++) {
            // TODO: Introduce clamp
            // float r = perlin.accumulatedOctaveNoise2D_0_1(x / fx, z / fz, perlin_octaves);
            float r = perlin.accumulatedOctaveNoise2D_0_1(x / fx, z / fz, perlin_octaves);
            // += as it builds upon encourager's height values
            hmap[(z * worldDimX) + x] += (r * max_height) - (max_height / 2.0);
        }
    }
}

void Mesh::generateMesh() {
    // Procedural generation
    // generateGaussianHeightMap();
    generateHeightMap();

    /* Calculate plane verticies + indicies */
    // 3 coordinates for every vertex/point in tilemap
    vertsSize = 3 * (worldDimX + 1) * (worldDimZ + 1);
    verts = new float[vertsSize * sizeof(float)];

    // 2 triangles * 3 verts/tri * num tiles
    indiciesSize = 2 * 3 * worldDimX * worldDimZ;
    indicies = new GLushort[indiciesSize * sizeof(GLushort)];

    // Calculate verticies (+ 1 for tiles -> verticies)
    int w = 0;
    for (int z = 0; z < (worldDimZ + 1); z++) {
        for (int x = 0; x < (worldDimX + 1); x++) {
            
            // 3 coordinates per vertex
            int base = 3 * ((z * (worldDimX + 1)) + x);
            verts[base + 0] = x;
            verts[base + 1] = hmap[(z * worldDimX) + x] + offset;
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
            // std::cout << "(" << x << ", " << z << "):" << base << "\t(" << (int)indicies[base] << ", " << (int)indicies[base + 1] << ", " << (int)indicies[base + 2] << ") (" << (int)indicies[base + 3] << ", " << (int)indicies[base + 4] << ", " << (int)indicies[base + 5] << ")" << std::endl;
        }
    }

}

void Mesh::encourageMountain(int c_x, int c_z, float height, float width) {
    for (int z = 0; z < worldDimZ; z++) {
        for (int x = 0; x < worldDimX; x++) {
            // Apply gaussian curve to heightmap
            // += as it builds upon other encourager's/perlin noise height values
            hmap[(z * worldDimX) + x] += gaussian2D(x, z, c_x, c_z, height, width, width);
        }
    }
}

void Mesh::encourageLake(int c_x, int c_z, float width) {
    for (int z = 0; z < worldDimZ; z++) {
        for (int x = 0; x < worldDimX; x++) {
            // Apply negative gaussian curve to heightmap
            // += as it builds upon other encourager's/perlin noise height values
            hmap[(z * worldDimX) + x] += gaussian2D(x, z, c_x, c_z, -20.0, width, width);
        }
    }
}

void Mesh::encourageRiver(int x0, int z0, int x1, int z1) {
    std::cout << "test: " << distanceFromSegment(0, 10, 0, 0, 50, 50) << std::endl;
    for (int z = 0; z < worldDimZ; z++) {
        for (int x = 0; x < worldDimX; x++) {
            // Apply distance function of every point to line segment (river)
            // += as it builds upon other encourager's/perlin noise height values
            float d = distanceFromSegment(x, z, x0, z0, x1, z1);
            if (d < 1.0) 
                hmap[(z * worldDimX) + x] += -20.0;
            
        }
    }
}