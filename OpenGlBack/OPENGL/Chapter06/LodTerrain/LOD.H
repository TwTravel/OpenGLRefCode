#ifndef LOD_H
#define LOD_H

#include <iostream.h>
#include <fstream.h>
#include <math.h>

#define DISTANCE(v1, v2) sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + \
                              (v1[2] - v2[2]) * (v1[2] - v2[2]))

#define MAP 256
#define QUAD_MAP (MAP + 1)

#define IX_QT(x, z) (((x)%QUAD_MAP) + (((z)%QUAD_MAP) * QUAD_MAP))

#define EDGE_POINT 0
#define NODE_POINT 1
#define UNKNOWN    2

#define NORTH   0
#define WEST    1
#define SOUTH   2
#define EAST    3
#define NW      4
#define NE      5
#define SW      6
#define SE      7
#define NORTH_L 8
#define NORTH_R 9
#define WEST_T  10
#define WEST_B  11
#define SOUTH_L 12
#define SOUTH_R 13
#define EAST_T  14
#define EAST_B  15

void reset_quad_tree(void);
void draw(int x, int z, int width, int direction);
void setup_quadtree(int x, int y, int width);

extern int quadtree[];

extern int location[];
extern int lod_level;

#endif

