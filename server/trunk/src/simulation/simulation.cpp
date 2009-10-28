#include "simulation.h"
#include "chipmunk.h"
#include "serialisebase.h"
#include "debug.h"
#include "body.h"
#include "shape.h"
#include "server.h"

using namespace WiredMunk;

Simulation::Simulation() {
	_space = NULL;
	
	_sampler = new PositionSampler();
	
	gettimeofday(&_lastRunTime, NULL);
	gettimeofday(&_lastSyncTime, NULL);
	
	cpInitChipmunk();
	cpResetShapeIdCounter();
}

Simulation::~Simulation() {
	delete _sampler;
}

void Simulation::run() {
	if (_space != NULL) {
		step();
		sync();
	}
}

void Simulation::sync() {
	
	// Calculate the time that has passed since the last time the clients were
	// synced
	struct timeval now;
	struct timeval timeDiff;
	
	gettimeofday(&now, NULL);
	
	timersub(&now, &_lastSyncTime, &timeDiff);
	
	// Do we need to resync the clients?
	if (timeDiff.tv_sec > RESYNC_SECONDS) {
		
		Debug::printf("Resyncing with clients\n");
		
		// Distribute the new simulation to all clients
		Server::getServer()->getClientManager()->sendSpace(_space);
		
		// Remember that we have synced all clients
		gettimeofday(&_lastSyncTime, NULL);
	}
}

void Simulation::step() {
		
	// Calculate the time that has passed since the last time the
	// simulation ran
	struct timeval thisRunTime;
	struct timeval timeDiff;
	
	gettimeofday(&thisRunTime, NULL);
	
	// Get time difference between last time and this time
	timersub(&thisRunTime, &_lastRunTime, &timeDiff);
	
	// Calculate time in 85ths of a second
	int steps = (((cpFloat)timeDiff.tv_usec) / 1000000.0) * 85.0;
	
	// Remember current run time if we are to step the simulation
	//if (steps > 0) _lastRunTime = thisRunTime;
	if (steps > 0) {
		struct timeval increaseTime;
		struct timeval nextRunTime;
		increaseTime.tv_usec = steps * (1000000.0 / 85.0);;
		timeradd(&_lastRunTime, &increaseTime, &nextRunTime);
		_lastRunTime = nextRunTime;
	}
	
	// Step the simulation
	cpFloat dt = 1.0/85.0;
	for (int i = 0; i < steps; ++i) {
		_space->step(dt);
		
		// Sample the simulation
		_sampler->sample(_space);
	}
}

void Simulation::handleMessageReceived(const Message& msg) {
	
	switch (msg.getType()) {
			
		case Message::MESSAGE_SPACE:
			handleSpaceReceived(msg);
			break;
			
		case Message::MESSAGE_BODY:
			handleBodyReceived(msg);
			break;
			
		case Message::MESSAGE_SHAPE:
			handleShapeReceived(msg);
			break;
			
		default:
			break;
	}
}

void Simulation::handleSpaceReceived(const Message& msg) {
	
	// Create a space on the server
	Debug::printf("Space data received\n");
	
	// Do we need to create a new space?
	if (_space == NULL) {
		
		// Create a new space and deserialise data into it
		_space = new Space(msg.getData());
	} else {
		
		// Space exists; deserialise data into existing space
		_space->deserialise(msg.getData());
	}
}

void Simulation::handleBodyReceived(const Message& msg) {
	
	// Create a body on the server
	Debug::printf("Body data received\n");
	
	// Abort if the space has not yet been initialised
	if (_space == NULL) return;
	
	// Deserialise the body in order to work out which local body it
	// represents
	Body* newBody = new Body(msg.getData());
	
	// Locate the existing body and update it
	for (int i = 0; i < _space->getBodies()->size(); ++i) {
		
		Body* oldBody = _space->getBodies()->at(i);
		
		// Found the body?
		if (newBody->getObjectId() == oldBody->getObjectId()) {
			
			// Located body - deserialise into it
			oldBody->deserialise(msg.getData());
		}
	}
	
	delete newBody;
	
	// Distribute the new simulation to all clients
	Server::getServer()->getClientManager()->sendSpace(_space);
	
	// Remember that we have synced all clients
	gettimeofday(&_lastSyncTime, NULL);
}

void Simulation::handleShapeReceived(const Message& msg) {
	
	// Create a shape on the server
	Debug::printf("Shape data received\n");
	
	
	// Abort if the space has not yet been initialised
	if (_space == NULL) return;
	
	// Deserialise the shape in order to work out which local shape it
	// represents
	Shape* newShape = new Shape(_space->getBodies(), _space->getStaticBodies(), msg.getData());
	bool isShape = false;
	
	// Locate the existing shape and update it
	for (int i = 0; i < _space->getShapes()->size(); ++i) {
		
		Shape* oldShape = _space->getShapes()->at(i);
		
		// Found the shape?
		if (newShape->getObjectId() == oldShape->getObjectId()) {
			
			// Located shape - deserialise into it
			oldShape->deserialise(_space->getBodies(), _space->getStaticBodies(), msg.getData());
			
			// Remember that the shape is a standard shape
			isShape = true;
		}
	}
	
	// Did we find the shape?
	if (!isShape) {
		
		// Shape must be a static shape
		// Locate the existing shape and update it
		for (int i = 0; i < _space->getStaticShapes()->size(); ++i) {
			
			Shape* oldShape = _space->getStaticShapes()->at(i);
			
			// Found the shape?
			if (newShape->getObjectId() == oldShape->getObjectId()) {
				
				// Located shape - deserialise into it
				oldShape->deserialise(_space->getBodies(), _space->getStaticBodies(), msg.getData());
			}
		}
	}
	
	delete newShape;
}
