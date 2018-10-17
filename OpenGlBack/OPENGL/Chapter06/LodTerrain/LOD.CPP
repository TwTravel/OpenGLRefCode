
#include <windows.h>
#include <iostream.h>
#include <stdlib.h>
#include <GL/gl.h>
#include "bmp_load.h"
#include "lod.h"

void draw_point(int , int , int , int);

int quadtree[QUAD_MAP * QUAD_MAP];

int location[3];
int lod_level = 8;

extern LAND_TEX lt;

void reset_quad_tree(void) 
{
	int i;
	
   for(i=0; i<(QUAD_MAP * QUAD_MAP); i++) 
   {
      quadtree[i] = UNKNOWN;
   }
}

void setup_quadtree(int x, int z, int width)
{
   int width2;
   int v1[3];
	
   v1[0] = x;
   v1[2] = z;
	
   width2 = width / 2;
	
   if((width > 1) && DISTANCE(v1, location) < width * lod_level) 
   {
      quadtree[IX_QT(x, z)] = NODE_POINT;
      quadtree[IX_QT(x - width2, z - width2)] = EDGE_POINT;
      quadtree[IX_QT(x + width2, z - width2)] = EDGE_POINT;
      quadtree[IX_QT(x - width2, z + width2)] = EDGE_POINT;
      quadtree[IX_QT(x + width2, z + width2)] = EDGE_POINT;
		
      setup_quadtree(x - width2, z - width2, width2);
      setup_quadtree(x + width2, z - width2, width2);
      setup_quadtree(x - width2, z + width2, width2);
      setup_quadtree(x + width2, z + width2, width2);
   }
   else 
   {
      quadtree[IX_QT(x, z)] = EDGE_POINT;
   }
}

void draw(int x, int z, int width, int direction) 
{
	int width2;

	if(width > 1) 
	{
		if(width > 16) 
		{
			glBindTexture(GL_TEXTURE_2D, lt.texture_name[(int)((x) / 64)][(int)((z) / 64)]);
		}
		width2 = width / 2;
		if(quadtree[IX_QT(x, z)] == NODE_POINT) 
		{
			// NORTH
			if(quadtree[IX_QT(x - width2, z - width2)] == EDGE_POINT &&
				quadtree[IX_QT(x + width2, z - width2)] == EDGE_POINT) 
			{
				
				if(quadtree[IX_QT(x, z - width*2)] == NODE_POINT || (z-width)%MAP==0) 
				{
					draw_point(x, z, width, NORTH_L);
					draw_point(x, z, width, NORTH_R);
				}
				else 
				{
					draw_point(x, z, width, NORTH);
				}
			}
			else if(quadtree[IX_QT(x - width2, z - width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, NORTH_L);
			}
			else if(quadtree[IX_QT(x + width2, z - width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, NORTH_R);
			}
			
			// SOUTH
			if(quadtree[IX_QT(x - width2, z + width2)] == EDGE_POINT &&
				quadtree[IX_QT(x + width2, z + width2)] == EDGE_POINT) 
			{
				
				if(quadtree[IX_QT(x, z + width*2)] == NODE_POINT || (z+width)%MAP==0) 
				{
					draw_point(x, z, width, SOUTH_L);
					draw_point(x, z, width, SOUTH_R);
				}
				else 
				{
					draw_point(x, z, width, SOUTH);
				}
			}
			else if(quadtree[IX_QT(x - width2, z + width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, SOUTH_L);
			}
			else if(quadtree[IX_QT(x + width2, z + width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, SOUTH_R);
			}
			
			// EAST
			if(quadtree[IX_QT(x + width2, z - width2)] == EDGE_POINT &&
				quadtree[IX_QT(x + width2, z + width2)] == EDGE_POINT) 
			{
				
				if(quadtree[IX_QT(x + width*2, z)] == NODE_POINT || (x+width)%MAP==0) 
				{
					draw_point(x, z, width, EAST_T);
					draw_point(x, z, width, EAST_B);
				}
				else 
				{
					draw_point(x, z, width, EAST);
				}
			}
			else if(quadtree[IX_QT(x + width2, z - width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, EAST_T);
			}
			else if(quadtree[IX_QT(x + width2, z + width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, EAST_B);
			}
			
			// WEST
			if(quadtree[IX_QT(x - width2, z - width2)] == EDGE_POINT &&
				quadtree[IX_QT(x - width2, z + width2)] == EDGE_POINT) 
			{
				
				if(quadtree[IX_QT(x - width*2, z)] == NODE_POINT || (x-width)%MAP==0) 
				{
					draw_point(x, z, width, WEST_T);
					draw_point(x, z, width, WEST_B);
				}
				else 
				{
					draw_point(x, z, width, WEST);
				}
			}
			else if(quadtree[IX_QT(x - width2, z - width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, WEST_T);
			}
			else if(quadtree[IX_QT(x - width2, z + width2)] == EDGE_POINT) 
			{
				draw_point(x, z, width, WEST_B);
			}
		}
		else 
		{
			return;
		}
		
		draw(x - width2, z - width2, width2, NW);
		draw(x + width2, z - width2, width2, NE);
		draw(x - width2, z + width2, width2, SW);
		draw(x + width2, z + width2, width2, SE);
	}
}
