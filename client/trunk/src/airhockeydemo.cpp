#include "airhockeydemo.h"
#include "debug.h"

using namespace WiredMunk;

AirHockeyDemo::AirHockeyDemo(const char* serverIP, int portNum) : WiredMunkApp(serverIP, portNum) {
}

void AirHockeyDemo::startup() {
	
	ticks = 0;
	
	_space = new WiredMunk::Space();
	_space->setIterations(20);
	_space->resizeStaticHash(40.0, 1000);
	_space->resizeActiveHash(40.0, 1000);
	_space->setGravity(cpv(0, 0));
	
	Body* staticBody = new Body(INFINITY, INFINITY);
	_space->addStaticBody(staticBody);
	
	// Create segments around the edge of the screen.
	Shape* shape = new Shape(staticBody, cpv(-320,-240), cpv(-320,240), 0.0f);
	shape->setElasticity(1.0);
	shape->setFriction(1.0);
	_space->addStaticShape(shape);
	
	shape = new Shape(staticBody, cpv(320,-240), cpv(320,240), 0.0f);
	shape->setElasticity(1.0);
	shape->setFriction(1.0);
	_space->addStaticShape(shape);
	
	shape = new Shape(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f);
	shape->setElasticity(1.0);
	shape->setFriction(1.0);
	_space->addStaticShape(shape);
	
	shape = new Shape(staticBody, cpv(-320,240), cpv(320,240), 0.0f);
	shape->setElasticity(1.0);
	shape->setFriction(1.0);
	_space->addStaticShape(shape);
	
	int num = 4;
	cpVect rectangleVerts[] = {
		cpv(-15,-50),
		cpv(-15, 50),
		cpv( 15, 50),
		cpv( 15,-50),
	};
	
	Body* body;
	
	for (int i = 0; i < 20; ++i) {
		body = new Body(10.0, 1.0);
		body->setPosition(cpv(32 + ((10 - i) * 8), -150 + (i * 16)));
		_space->addBody(body);
	
		shape = new Shape(body, 10 + i, cpv(0, 0));
		shape->setElasticity(0.0);
		shape->setFriction(0.8);
		_space->addShape(shape);
	}
	
	_body1 = new Body(10.0, cpMomentForPoly(1.0, num, rectangleVerts, cpvzero));
	_body1->setPosition(cpv(32, -150));
	_space->addBody(_body1);
	
	shape = new Shape(_body1, num, rectangleVerts, cpvzero);
	shape->setElasticity(0.0);
	shape->setFriction(0.8);
	_space->addShape(shape);
	
	_body2 = new Body(10.0, cpMomentForPoly(1.0, num, rectangleVerts, cpvzero));
	_body2->setPosition(cpv(32, -110 + 32));
	_space->addBody(_body2);
		
	shape = new Shape(_body2, num, rectangleVerts, cpvzero);
	shape->setElasticity(0.0);
	shape->setFriction(0.8);
	_space->addShape(shape);
}

void AirHockeyDemo::shutdown() {
	delete _space;
}

void AirHockeyDemo::runUser() {

}

void AirHockeyDemo::keyboard(unsigned char key, int x, int y) {
	
	// Choose the body to act on
	Body* body = (getClientId() == 1 ? _body1 : _body2);
	
	cpVect force;
	cpVect rot;
	
	switch (key) {
		case 119:
			force.x = 0;
			force.y = 200;
			break;
		case 97:
			force.x = -200;
			force.y = 0;
			break;
		case 100:
			force.x = 200;
			force.y = 0;
			break;
		case 115:
			force.x = 0;
			force.y = -200;
			break;
	}
	
	rot.x = 0;
	rot.y = 0;

	body->applyImpulse(force, rot);
}
