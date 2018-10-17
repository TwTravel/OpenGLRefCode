#ifndef USEFUL_H
#define USEFUL_H


#ifndef PI
#define PI 3.1415926535
#endif PI

#define COMP 128 // This is used for scaling the terrain coordinates.
#define SHIFT 7 // This is the same, but can be used on binary shift operations(optimization).

#define M_SIZE 256 // The maximum map size.
#define IX(x, z) (((x)&255) + (((z)&255) << 8)) // A method of converting a 2D coordinate to 1D.

typedef struct COLOUR 
{
	int r,g,b;
} COLOUR;

#endif USEFUL_H