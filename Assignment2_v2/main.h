#include "Leap.h"
#include "LeapC.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <gl/freeglut.h>
#include <cstring>

#define OBJ_WIREFRAME	0
#define OBJ_SOLID		1
#define OBJ_EDGE		2 

struct HE_edge;
struct HE_vert;
struct HE_face;
struct HE_edge {
	HE_vert* vert; // vertex at the end of the half-edge
	HE_edge* pair; // oppositely oriented half-edge
	HE_face* face; // the incident face	
	HE_edge* prev; // previous half-edge around the face
	HE_edge* next; // next half-edge around the face
	unsigned int id;
};

struct HE_vert {
	float x, y, z; // the vertex coordinates
	float nom_x, nom_y, nom_z; // the norm values
	HE_edge* edge; // one of the half-edges emanating from the vertex
	unsigned int id;
};

struct HE_face {
	HE_edge* edge; // one of the half-edges bordering the face
	unsigned int id;

};


static int win;
static int menid;
static int submenid;
static int primitive = 0;
static int prev_x, prev_y, prev_z;
static float prev_tr_x, prev_tr_y;
static float x_angle = 0.0;
static float y_angle = 0.0;
static float z_angle = 0.0;
static float scale_size = 1;
static int obj_mode = 0;

static int face_num = 80000;			
static int ver_num = 40002;				
static int he_num = face_num * 3;		//total number of half edges 
float t = 0.0f;

static int rotateGestureDetected = 0;	 
static int ZoomGestureDetected = 0;
static int TranslateGestureDetected = 0;
static float  tx = 0.0;
static float  ty = 0.0;
static float  tz = 0.0;
const std::string fingerNames[] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const std::string boneNames[] = { "Metacarpal", "Proximal", "Middle", "Distal" };
