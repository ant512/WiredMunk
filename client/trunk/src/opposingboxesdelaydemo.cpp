#include "opposingboxesdelaydemo.h"
#include "debug.h"

using namespace WiredMunk;

OpposingBoxesDelayDemo::OpposingBoxesDelayDemo(const char* serverIP, int portNum) : WiredMunkApp(serverIP, portNum) {
}

void OpposingBoxesDelayDemo::startup() {
	
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
	
	int num = 4;
	cpVect verts[] = {
		cpv(-15,-15),
		cpv(-15, 15),
		cpv( 15, 15),
		cpv( 15,-15),
	};
	
	for (int i = 0; i < 8; ++i) {
		Body* body = body = new Body(1.0, cpMomentForPoly(1.0, num, verts, cpvzero));
		body->setPosition(cpv(32, -150 + (i * 32)));
		_space->addBody(body);
		
		shape = new Shape(body, num, verts, cpvzero);
		shape->setElasticity(0.0);
		shape->setFriction(0.8);
		_space->addShape(shape);
	}
	
	_body1 = _space->getBodies()->at(0);
	_body2 = _space->getBodies()->at(_space->getBodies()->size() - 1);
}

void OpposingBoxesDelayDemo::shutdown() {
	delete _space;
}

void OpposingBoxesDelayDemo::runUser() {
	
	ticks++;
	
	int tickCount = (getClientId() == 1 ? 10 : 50);
	
	if (ticks == tickCount) {
		
		ticks = 0;
	
		// Choose the body to act on
		Body* body = (getClientId() == 1 ? _body1 : _body2);
		
		cpVect force;
		cpVect rot;
		
		if (getClientId() == 1) {
			force.x = 0;
			force.y = 20;
		} else {
			force.x = 0;
			force.y = -20;
		}
		
		rot.x = 0;
		rot.y = 0;
		
		body->applyImpulse(force, rot);
	}
}

void OpposingBoxesDelayDemo::keyboard(unsigned char key, int x, int y) {
	
	// Choose the body to act on
	Body* body = (getClientId() == 1 ? _body1 : _body2);
	
	cpVect force;
	cpVect rot;
	
	if (getClientId() == 1) {
		force.x = 0;
		force.y = 20;
	} else {
		force.x = 0;
		force.y = -20;
	}
	
	rot.x = 0;
	rot.y = 0;
	
	body->applyImpulse(force, rot);
	
	/*	
	switch (key) {
		case 119:
			force.x = 0;
			force.y = 20;
			break;
		case 97:
			force.x = -20;
			force.y = 0;
			break;
		case 100:
			force.x = 20;
			force.y = 0;
			break;
		case 115:
			force.x = 0;
			force.y = -20;
			break;
	}
	
	rot.x = 0;
	rot.y = 0;

	body->applyImpulse(force, rot);
	 
	 */
}
