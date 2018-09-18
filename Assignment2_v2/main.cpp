/* Program to render the given 3D mesh using OpenGL and interact using LeapMotion*/
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <GL/freeglut.h>
#include <cstring>
#include "Leap.h"
#include "LeapC.h"
#include "main.h"
using namespace std;
using namespace Leap;

Controller controller;

void rotate_motion(int x, int y, int z)
{	
		x_angle += (x - prev_x) / 5.0;
		if (x_angle > 180)
			x_angle -= 360;
		else if (x_angle < -180)
			x_angle += 360;
		prev_x = x;
		y_angle += (y - prev_y) / 5.0;
		if (y_angle > 180)
			y_angle -= 360;
		else if (y_angle < -180)
			y_angle += 360;
		prev_y = y;
		z_angle += (z - prev_z) / 5.0;
		if (z_angle > 180)
			z_angle -= 360;
		else if (z_angle < -180)
			z_angle += 360;
		prev_z = z;
		cout << "x_angle, y_angle, z_angle =" << x_angle << " " << y_angle << " " << z_angle;
	
}

void zoom_motion(int x, int y, int z)
{
		float old_size = scale_size;
		scale_size *= (1 - (z - prev_y) / 30.0);
		cout << "old_size = " << old_size << endl;
		cout << "scale_size = " << scale_size << endl;
		if (scale_size <0)
			scale_size = old_size;
		prev_y = z;	
}

void translate_motion(float x, float y, float z)
{

	tx = (prev_tr_x - x) / 60; 
	ty = (prev_tr_x - y) / 60;
	
	cout << "Translate: tx, ty, tz = " << tx << ty << tz;
}

void onInit(const Controller&);
void onConnect(const Controller&);
void onDisconnect(const Controller&);
void onExit(const Controller&);
void onFrame(const Controller&);
void onFocusGained(const Controller&);
void onFocusLost(const Controller&);
void onDeviceChange(const Controller&);
void onServiceConnect(const Controller&);
void onServiceDisconnect(const Controller&);
void onServiceChange(const Controller&);
void onDeviceFailure(const Controller&);
void onLogMessage(const Controller&, MessageSeverity severity, int64_t timestamp, const char* msg);

void onInit(const Controller& controller) {
}

void onConnect(const Controller& controller) {
}

void onDisconnect(const Controller& controller) {
	std::cout << "Disconnected" << std::endl;
}

void onExit(const Controller& controller) {
	std::cout << "Exited" << std::endl;
}

void onFrame(const Controller& controller) {
	const Frame frame = controller.frame();
	int print_once = 0;
	HandList hands = frame.hands();
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		const Hand hand = *hl;
		if (!hands.isEmpty()) {
			if (hands.count() == 1) {	
				if (hand.isRight()) {
					ZoomGestureDetected = 0;
					TranslateGestureDetected = 0;
					cout << "rotate gesture Detected" << endl;
					Vector handCenter = hand.palmPosition();			
					rotate_motion(handCenter.x, handCenter.y, handCenter.z);
				}
				if (hand.isLeft()) {
					ZoomGestureDetected = 0;
					rotateGestureDetected = 0;
					cout << "Translate gesture Detected" << endl;
					TranslateGestureDetected = 1;
					Vector handCenter = hand.palmPosition();					
					translate_motion(handCenter.x, handCenter.y, handCenter.z);
				}
			}		
			else if (hands.count() == 2) {				
				if (hand.isRight()) {
					rotateGestureDetected = 0;
					TranslateGestureDetected = 0;
					cout << "Zoom gesture Detected" << endl;
					ZoomGestureDetected = 1;
					Vector handCenter = hand.palmPosition();	
					zoom_motion(handCenter.x, handCenter.y, handCenter.z);
				}
			}
		}
	}
}


void onFocusGained(const Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void onFocusLost(const Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}

void onDeviceChange(const Controller& controller) {
	std::cout << "Device Changed" << std::endl;
	const DeviceList devices = controller.devices();
	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
		std::cout << "  isSmudged:" << (devices[i].isSmudged() ? "true" : "false") << std::endl;
		std::cout << "  isLightingBad:" << (devices[i].isLightingBad() ? "true" : "false") << std::endl;
	}
}

void onServiceConnect(const Controller& controller) {
	std::cout << "Service Connected" << std::endl;
}

void onServiceDisconnect(const Controller& controller) {
	std::cout << "Service Disconnected" << std::endl;
}

void onServiceChange(const Controller& controller) {
	std::cout << "Service Changed" << std::endl;
}

void onDeviceFailure(const Controller& controller) {
	std::cout << "Device Error" << std::endl;
	const Leap::FailedDeviceList devices = controller.failedDevices();

	for (FailedDeviceList::const_iterator dl = devices.begin(); dl != devices.end(); ++dl) {
		const FailedDevice device = *dl;
		std::cout << "  PNP ID:" << device.pnpId();
		std::cout << "    Failure type:" << device.failure();
	}
}

void onLogMessage(const Controller&, MessageSeverity s, int64_t t, const char* msg) {
	switch (s) {
	case Leap::MESSAGE_CRITICAL:
		std::cout << "[Critical]";
		break;
	case Leap::MESSAGE_WARNING:
		std::cout << "[Warning]";
		break;
	case Leap::MESSAGE_INFORMATION:
		std::cout << "[Info]";
		break;
	case Leap::MESSAGE_UNKNOWN:
		std::cout << "[Unknown]";
	}
	std::cout << "[" << t << "] ";
	std::cout << msg << std::endl;
}

HE_face* face = new HE_face[face_num];
HE_vert* vert = new HE_vert[ver_num];
HE_edge* halfEdge = new HE_edge[he_num];

void readAndStore()
{
	ifstream infile;
	char c_data[100];
	int face_pro;
	unsigned int i = 0;
	unsigned int j = 0;
	int vert_count = 0;
	int face_count = 0;
	int he_num = 0;
	HE_vert* vert_id_0;
	HE_vert* vert_id_1;
	HE_vert* vert_id_2;
	int result = 0;
	unsigned int temp = 0;
	face_pro = 0;
	infile.open("bunny_new.m");
	while (!infile.eof()) {
		infile >> c_data; 
		if (strcmp(c_data, "Face") == 0) { face_pro = 1; };
		if (face_pro == 0) { 
			result = i % 8;
			switch (result) {
			case 2: vert[vert_count].x = atof(c_data);
				break;
			case 3: vert[vert_count].y = atof(c_data);
				break;
			case 4: vert[vert_count].z = atof(c_data);
				break;
			case 5: vert[vert_count].nom_x = atof(c_data);
				break;
			case 6: vert[vert_count].nom_y = atof(c_data);
				break;
			case 7: vert[vert_count].nom_z = atof(c_data);
				break;
			case 0: if (i != 0) { vert_count++; };
					break; 
			}
			vert[vert_count].id = vert_count;
			i++;
		}
		else { 
			result = j % 5;
			temp = atoi(c_data);
			switch (result) {
			case 2: vert_id_0 = &vert[temp - 1];
				break;
			case 3: vert_id_1 = &vert[temp - 1];
				break;
			case 4: vert_id_2 = &vert[temp - 1];
				break;
			case 0: if (j != 0) {
				halfEdge[he_num].face = &face[face_count];     
				halfEdge[he_num + 1].face = &face[face_count]; 
				halfEdge[he_num + 2].face = &face[face_count]; 
				halfEdge[he_num].prev = &halfEdge[he_num + 2];
				halfEdge[he_num].next = &halfEdge[he_num + 1];
				halfEdge[he_num + 1].prev = &halfEdge[he_num];
				halfEdge[he_num + 1].next = &halfEdge[he_num + 2];
				halfEdge[he_num + 2].prev = &halfEdge[he_num + 1];
				halfEdge[he_num + 2].next = &halfEdge[he_num];
				halfEdge[he_num].vert = vert_id_1;
				halfEdge[he_num + 1].vert = vert_id_2;
				halfEdge[he_num + 2].vert = vert_id_0;
				halfEdge[he_num].id = he_num;
				halfEdge[he_num + 1].id = he_num + 1;
				halfEdge[he_num + 2].id = he_num + 2;
				halfEdge[he_num].pair = NULL; 
				halfEdge[he_num + 1].pair = NULL; 
				halfEdge[he_num + 2].pair = NULL; 
        		vert_id_0->edge = &halfEdge[he_num];
				vert_id_1->edge = &halfEdge[he_num + 1];
				vert_id_2->edge = &halfEdge[he_num + 2];
				face[face_count].edge = &halfEdge[he_num];
				face_count++;
				he_num = he_num + 3;
			};
					break; 
			}
			face[face_count].id = face_count;
			j++;
		}
	}
	infile.close();
}

HE_face* getFaceIdOppositedOrientedVert(HE_vert* vert_0, HE_vert* vert_1)
{
	int i = 0;
	HE_face* face;
	HE_vert* vert_id_0;
	HE_vert* vert_id_1;
	HE_vert* vert_id_2;
	while (i<he_num) {
		face = halfEdge[i].face;
		vert_id_0 = halfEdge[i].vert;
		vert_id_1 = halfEdge[i + 1].vert;
		vert_id_2 = halfEdge[i + 2].vert;
		if (vert_id_1->id == vert_0->id && vert_id_0->id == vert_1->id) { return face; };
		if (vert_id_0->id == vert_0->id && vert_id_2->id == vert_1->id) { return face; };
		if (vert_id_2->id == vert_0->id && vert_id_1->id == vert_1->id) { return face; };
		i = i + 3;
	}
	return NULL;
}

void constructMesh()
{
	int i = 0;
	int he_num = 0;
	int halfedge_cnt = 0;
	HE_edge* halfedge_cnt_id = NULL;
	HE_face* oppo_found = NULL;
	HE_vert* tmp_vert_1 = NULL;
	HE_vert* tmp_vert_2 = NULL;
	HE_edge* tmp_he_1 = NULL;
	i = 0;
	while (i < he_num) {
		tmp_vert_1 = halfEdge[i].vert;				
		tmp_vert_2 = halfEdge[i].prev->vert;			
		oppo_found = getFaceIdOppositedOrientedVert(tmp_vert_2, tmp_vert_1);
		if (oppo_found != NULL) {			
			tmp_he_1 = oppo_found->edge;	
													
			while (tmp_he_1->vert != tmp_vert_2) {
				tmp_he_1 = tmp_he_1->next;
			}

			halfEdge[i].pair = tmp_he_1;
		}
		else { halfEdge[i].pair = NULL; };		
		i++;
	}
}

void draw_axis_3D(void)
{
	GLUquadric* cyl = gluNewQuadric();
	gluCylinder(cyl, 0.02, 0.02, 0.8, 16, 1);        
	glColor3f(0.2, 0.3, 0.4);                              
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.8);
	gluCylinder(cyl, 0.04, 0.001, 0.08, 12, 1);  
	glPopMatrix();
}
void draw_XYZ_axis(void) {
	GLUquadric* cyl = gluNewQuadric();
	glColor3f(0.3, 0.3, 1.0);    // Z axis is blue.
	draw_axis_3D();
	glColor3f(0.3, 1.0, 0.3);    // Y axis is green.
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	draw_axis_3D();
	glPopMatrix();
	glColor3f(1.0, 0.3, 0.3);    // X axis is red.
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	draw_axis_3D();
	glPopMatrix();
}
void menu(int value)
{	if (value == 0)
	{
		glutDestroyWindow(win);
		exit(0);
	}
	else
	{
		primitive = value;
	}
	glutPostRedisplay();
}


void createmenu(void)
{
	submenid = glutCreateMenu(menu);
	glutAddMenuEntry("Teapot", 2);
	glutAddMenuEntry("Cube", 3);
	glutAddMenuEntry("Torus", 4);
	menid = glutCreateMenu(menu);
	glutAddMenuEntry("Clear", 1);
	glutAddSubMenu("Draw", submenid);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void draw_XY_ground_plane(void)
{
	glColor3f(.9, .9, .9);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glVertex3f(10, 0, 10);
	glVertex3f(10, 0, 0);
	glEnd();
	glColor3f(.9, .9, .9);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, -10);
	glVertex3f(-10, 0, -10);
	glVertex3f(-10, 0, 0);
	glEnd();
	glColor3f(.9, .9, .9);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glVertex3f(-10, 0, 10);
	glVertex3f(-10, 0, 0);
	glEnd();
	glColor3f(.9, .9, .9);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, -10);
	glVertex3f(10, 0, -10);
	glVertex3f(10, 0, 0);
	glEnd();
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		if (i == 0) { glColor3f(.6, .3, .3); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, 10);
		if (i == 0) { glColor3f(.3, .3, .6); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(0, 0, i);
		glVertex3f(10, 0, i);
	};
	glEnd();
	glBegin(GL_LINES);
	for (int i = 0; i >= -10; i--) {
		if (i == 0) { glColor3f(.6, .3, .3); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, -10);
		if (i == 0) { glColor3f(.3, .3, .6); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(0, 0, i);
		glVertex3f(-10, 0, i);
	};
	glEnd();
	int j = 0;
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		if (i == 0) { glColor3f(.6, .3, .3); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(i*(-1), 0, 0);
		glVertex3f(i*(-1), 0, 10);
		if (i == 0) { glColor3f(.3, .3, .6); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(0, 0, i);
		glVertex3f(-10, 0, i);
	};
	glEnd();
	glBegin(GL_LINES);
	for (int i = 0; i <= 10; i++) {
		if (i == 0) { glColor3f(.6, .3, .3); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, -10);
		if (i == 0) { glColor3f(.3, .3, .6); }
		else { glColor3f(.25, .25, .25); };
		glVertex3f(0, 0, i*(-1));
		glVertex3f(10, 0, i*(-1));
	};
	glEnd();
}



void display(void) //glVertex function 
{
	HE_edge* outgoing_he;
	HE_edge* curr;
	GLfloat white[] = { 1., 1., 1., 1. };
	GLfloat light0_position[] = { 1., 1., 5., 0. };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	//Give color to the lighting
	GLfloat white8[] = { .8, .8, .8, 1. };
	GLfloat white2[] = { .2,.2,.2,1. };
	GLfloat black[] = { 0.,0.,0. };
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat lightColor[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	//GLfloat lightPos[] = { 10,0,-15,1 };
	GLfloat lightPos[] = { 15,15,15,15 };
	GLfloat mat_shininess[] = { 50 }; // Phong exponent
	GLfloat materialColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//The specular (shiny) component of the material
	GLfloat materialSpecular[] = { 0,0,1,1 };
	//The color emitted by the material
	GLfloat materialEmission[] = { 1.0f,1.0f,0, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialEmission);
	//Enable lighting and color of the light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	// setup the perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, .1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
	onInit(controller);
	onConnect(controller);
	onFrame(controller);
	// rotate, scale, translate the object
	glTranslatef(tx, ty, 0.0f); //HX
	glRotatef(x_angle, 0, 1, 0);
	glRotatef(y_angle, 1, 0, 0);
	glScalef(scale_size, scale_size, scale_size);
	//draw the 3D model at smooth shading model 
	glShadeModel(GL_SMOOTH);
	for (int i = 0; i < face_num; i++)
	{
		outgoing_he = face[i].edge;
		curr = outgoing_he;
		glBegin(GL_TRIANGLES);
		glColor3f(0, 1, 0);
		glNormal3f(curr->vert->nom_x, curr->vert->nom_y, curr->vert->nom_z);
		glVertex3f(curr->vert->x, curr->vert->y, curr->vert->z);
		//glVertex3f(0.1, 0.15, 0.66);
		glColor3f(0, 1, 0);
		glNormal3f(curr->next->vert->nom_x, curr->next->vert->nom_y, curr->next->vert->nom_z);
		glVertex3f(curr->next->vert->x, curr->next->vert->y, curr->next->vert->z);
		//glVertex3f(0.2, 0.3, 0.4);
		glColor3f(0, 1, 0);
		glNormal3f(curr->next->next->vert->nom_x, curr->next->next->vert->nom_y, curr->next->next->vert->nom_z);
		glVertex3f(curr->next->next->vert->x, curr->next->next->vert->y, curr->next->next->vert->z);
		//glVertex3f(0.7, 0.8, 0.9);
		glEnd();
	}
	//Disable the lighting
	glDisable(GL_LIGHTING);
	//Draw X Y ground plane
	draw_XY_ground_plane();
	//Draw  x y z axis cone
	draw_XYZ_axis();
	glutPostRedisplay();
	// swap the buffers
	glutSwapBuffers();
}

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	win = glutCreateWindow("3D Mesh Interaction");
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	readAndStore();
	constructMesh();
	createmenu();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glutDisplayFunc(display);
	glutMainLoop();
	return 1;
}