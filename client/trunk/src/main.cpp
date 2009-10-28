// OpenGL code taken from demo application supplied with Chipmunk

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
	#include "OpenGL/gl.h"
	#include "OpenGL/glu.h"
	#include <GLUT/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif

#include "airhockeydemo.h"
#include "body.h"
#include "chipmunk.h"
#include "shape.h"
#include "space.h"

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_PORT_NUMBER 4444
#define SLEEP_TICKS 16

using namespace WiredMunk;

AirHockeyDemo* app;
int portNumber;
char serverIP[16];

static void drawCircle(cpFloat x, cpFloat y, cpFloat r, cpFloat a)
{
	const int segs = 15;
	const cpFloat coef = 2.0*M_PI/(cpFloat)segs;
	
	glBegin(GL_LINE_STRIP); {
		for(int n = 0; n <= segs; n++){
			cpFloat rads = n*coef;
			glVertex2f(r*cos(rads + a) + x, r*sin(rads + a) + y);
		}
		glVertex2f(x,y);
	} glEnd();
}

static void
drawCircleShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpCircleShape *circle = (cpCircleShape *)shape;
	cpVect c = cpvadd(body->p, cpvrotate(circle->c, body->rot));
	drawCircle(c.x, c.y, circle->r, body->a);
}

static void
drawSegmentShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpSegmentShape *seg = (cpSegmentShape *)shape;
	cpVect a = cpvadd(body->p, cpvrotate(seg->a, body->rot));
	cpVect b = cpvadd(body->p, cpvrotate(seg->b, body->rot));
	
	glBegin(GL_LINES); {
		glVertex2f(a.x, a.y);
		glVertex2f(b.x, b.y);
	} glEnd();
}

static void
drawPolyShape(cpShape *shape)
{
	cpBody *body = shape->body;
	cpPolyShape *poly = (cpPolyShape *)shape;
	
	int num = poly->numVerts;
	cpVect *verts = poly->verts;
	
	glBegin(GL_LINE_LOOP);
	for(int i=0; i<num; i++){
		cpVect v = cpvadd(body->p, cpvrotate(verts[i], body->rot));
		glVertex2f(v.x, v.y);
	} glEnd();
}

static void
drawObject(void *ptr, void *unused)
{
	cpShape *shape = (cpShape *)ptr;
	switch(shape->klass->type){
		case CP_CIRCLE_SHAPE:
			drawCircleShape(shape);
			break;
		case CP_SEGMENT_SHAPE:
			drawSegmentShape(shape);
			break;
		case CP_POLY_SHAPE:
			drawPolyShape(shape);
			break;
		default:
			printf("Bad enumeration in drawObject().\n");
	}
}

static void
drawBB(void *ptr, void *unused)
{
	cpShape *shape = (cpShape *)ptr;
	
	glBegin(GL_LINE_LOOP); {
		glVertex2f(shape->bb.l, shape->bb.b);
		glVertex2f(shape->bb.l, shape->bb.t);
		glVertex2f(shape->bb.r, shape->bb.t);
		glVertex2f(shape->bb.r, shape->bb.b);
	} glEnd();
}

static void
drawCollisions(void *ptr, void *data)
{
	cpArbiter *arb = (cpArbiter *)ptr;
	for(int i=0; i<arb->numContacts; i++){
		cpVect v = arb->contacts[i].p;
		glVertex2f(v.x, v.y);
	}
}

static void 
pickingFunc(cpShape *shape, void *data)
{
	drawObject(shape, NULL);
}

static void
display(void)
{
	if (WiredMunkApp::getApp()->getClientState() != WiredMunkApp::CLIENT_STATE_RUNNING) return;
	
	Space* space = WiredMunkApp::getApp()->getSpace();
	
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw shapes
	glColor3f(0.0, 0.0, 0.0);
	for (int i = 0; i < space->getShapes()->size(); ++i) {
		drawObject(space->getShapes()->at(i)->getShape(), NULL);
	}
	for (int i = 0; i < space->getStaticShapes()->size(); ++i) {
		drawObject(space->getStaticShapes()->at(i)->getShape(), NULL);
	}
	
	// Draw bodies
	BodyVector* bodies = space->getBodies();
	
	glBegin(GL_POINTS); {
		glColor3f(0.0, 0.0, 1.0);
		for (int i = 0; i < bodies->size(); ++i){
			cpBody* body = (cpBody*)bodies->at(i)->getBody();
			glVertex2f(body->p.x, body->p.y);
		}
		
		glColor3f(1.0, 0.0, 0.0);
		cpArrayEach(space->getSpace()->arbiters, &drawCollisions, NULL);
	} glEnd();
	
	glutSwapBuffers();
}

static void keyboard(unsigned char key, int x, int y) {
	app->keyboard(key, x, y);
}

static void
timercall(int value)
{
	glutTimerFunc(SLEEP_TICKS, timercall, 0);
	
	WiredMunkApp::getApp()->run();
	
	glutPostRedisplay();
}

static void
idle(void)
{
	glutPostRedisplay();
}

static void
initGL(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	
	glPointSize(3.0);
	
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth(2.5f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-320.0, 320.0, -240.0, 240.0, -1.0, 1.0);
	glTranslatef(0.5, 0.5, 0.0);
}

static void
glutStuff(int argc, const char *argv[])
{
	glutInit(&argc, (char**)argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	glutInitWindowSize(640, 480);
	glutCreateWindow("WiredMunkDemo");
	
	initGL();
	
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(SLEEP_TICKS, timercall, 0);
	
	glutMainLoop();
}

void getCommandLineArgs(int argc, const char* argv[]) {
	
	// Set defaults
	portNumber = DEFAULT_PORT_NUMBER;
	strcpy(serverIP, DEFAULT_SERVER_IP);
	
	// Get settings from command line
	for (int i = 0; i < argc; ++i) {
		if (strncmp(argv[i], "-p", 2) == 0) {
			portNumber = atoi(argv[i + 1]);
		} else if (strncmp(argv[i], "-s", 2) == 0) {
			strcpy(serverIP, argv[i + 1]);
		} else if (strncmp(argv[i], "-h", 2) == 0) {
			printf("Usage: %s [-s serverIP] [-p port]\n", argv[0]);
			exit(0);
		}
	}
}

int main (int argc, const char* argv[]) {
	
	getCommandLineArgs(argc, argv);
    
	// Create WiredMunk application
	app = new AirHockeyDemo(serverIP, portNumber);
	
	glutStuff(argc, argv);
	
    return 0;
}
