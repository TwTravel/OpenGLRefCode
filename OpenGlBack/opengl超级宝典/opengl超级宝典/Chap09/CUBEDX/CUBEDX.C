// CubeDX.c
// OpenGL SuperBible, Chapter 9
// Demonstrates rendering with Indexed Vertex Arrays
// Program by Richard S. Wright Jr.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>


// Array containing the six vertices of the cube
static GLfloat corners[] = { -25.0f, 25.0f, 25.0f, // 0 // Front of cube
                              25.0f, 25.0f, 25.0f, // 1
                              25.0f, -25.0f, 25.0f,// 2
                             -25.0f, -25.0f, 25.0f,// 3
                             -25.0f, 25.0f, -25.0f,// 4  // Back of cube
                              25.0f, 25.0f, -25.0f,// 5
                              25.0f, -25.0f, -25.0f,// 6
                             -25.0f, -25.0f, -25.0f };// 7

// Array of indexes to create the cube
static GLubyte indexes[] = { 0, 1, 2, 3,	// Front Face
							 4, 5, 1, 0,	// Top Face
							 3, 2, 6, 7,	// Bottom Face
							 5, 4, 7, 6,	// Back Face
							 1, 5, 6, 2,	// Right Face
							 4, 0, 3, 7 };	// Left Face

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;



// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
    {
    float length;
    
    // Calculate the length of the vector       
    length = (float)sqrt((vector[0]*vector[0]) + 
                        (vector[1]*vector[1]) +
                        (vector[2]*vector[2]));

    // Keep the program from blowing up by providing an exceptable
    // value for vectors that may calculated too close to zero.
    if(length == 0.0f)
        length = 1.0f;

    // Dividing each element by the length will result in a
    // unit normal vector.
    vector[0] /= length;
    vector[1] /= length;
    vector[2] /= length;
    }


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
    {
    float v1[3],v2[3];
    static const int x = 0;
    static const int y = 1;
    static const int z = 2;

    // Calculate two vectors from the three points
    v1[x] = v[0][x] - v[1][x];
    v1[y] = v[0][y] - v[1][y];
    v1[z] = v[0][z] - v[1][z];

    v2[x] = v[1][x] - v[2][x];
    v2[y] = v[1][y] - v[2][y];
    v2[z] = v[1][z] - v[2][z];

    // Take the cross product of the two vectors to get
    // the normal vector which will be stored in out
    out[x] = v1[y]*v2[z] - v1[z]*v2[y];
    out[y] = v1[z]*v2[x] - v1[x]*v2[z];
    out[z] = v1[x]*v2[y] - v1[y]*v2[x];

    // Normalize the vector (shorten length to one)
    ReduceToUnit(out);
    }



// Called to draw scene
void RenderScene(void)
    {
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make the cube a wire frame
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
  
	// Save the matrix state
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    // Rotate about x and y axes
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 0.0f, 1.0f);

    // Enable and specify the vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, corners);
       
    // Using Drawarrays
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indexes);

  
    glPopMatrix();

    // Swap buffers
    glutSwapBuffers();
    }


// This function does any needed initialization on the rendering
// context. 
void SetupRC()
    {
    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	glColor3ub(0,0,255);
    }

///////////////////////////////////////////////////////////////////////////////
// Process arrow keys
void SpecialKeys(int key, int x, int y)
    {
    if(key == GLUT_KEY_UP)
        xRot-= 5.0f;

    if(key == GLUT_KEY_DOWN)
        xRot += 5.0f;

    if(key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if(key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    if(key > 356.0f)
        xRot = 0.0f;

    if(key < -1.0f)
        xRot = 355.0f;

    if(key > 356.0f)
        yRot = 0.0f;

    if(key < -1.0f)
        yRot = 355.0f;

    // Refresh the Window
    glutPostRedisplay();
    }


void ChangeSize(int w, int h)
    {
    GLfloat nRange = 100.0f;

    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    // Reset coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Establish clipping volume (left, right, bottom, top, near, far)
    if (w <= h) 
        glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange*2.0f, nRange*2.0f);
    else 
        glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange*2.0f, nRange*2.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    }

int main(int argc, char* argv[])
    {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Cube DX");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);

	SetupRC();

	glutMainLoop();
    return 0;
    }
