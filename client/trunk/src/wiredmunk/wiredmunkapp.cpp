#include "wiredmunkapp.h"
#include "chipmunk.h"
#include "serialisebase.h"
#include "debug.h"
#include "body.h"
#include "shape.h"
#include "joint.h"
#include "positionsampler.h"

using namespace WiredMunk;

WiredMunkApp* WiredMunkApp::_singleton = NULL;

WiredMunkApp::WiredMunkApp(const char* serverIP, int portNum) {
	_socket.open(serverIP, portNum);
	_socket.addSocketEventHandler(this);
	_singleton = this;
	_clientState = CLIENT_STATE_NEW;
	_space = NULL;
	
	gettimeofday(&_lastRunTime, NULL);
	
	cpInitChipmunk();
	cpResetShapeIdCounter();
	
	requestHandshake();
}

WiredMunkApp::~WiredMunkApp() {
	shutdown();
	
	_socket.shut();
}

void WiredMunkApp::run() {
	
	// Check for incoming messages
	_socket.poll();
	
	// Choose actions based on current state
	switch (_clientState) {
		case CLIENT_STATE_NEW:
			
			// Should never be in this state at this point
			break;
			
		case CLIENT_STATE_WAITING_HANDSHAKE:
			
			// Waiting for a handshake response from the server.  Do nothing
			break;
			
		case CLIENT_STATE_WAITING_STARTUP:
			
			// Waiting for permission to startup from the server. Do nothing
			break;
			
		case CLIENT_STATE_STARTING:
			
			// Handshake received, so call the startup method
			startup();
			
			// Ensure all objects are reset to unaltered state
			resetAlteredState();
			
			// Send the space to the server
			sendSpace();
			
			// Inform the server that the client is ready to start
			sendReady();
			
			_sampler = new PositionSampler();
			
			break;
			
		case CLIENT_STATE_WAITING_READY:
			
			// Waiting for a commencement message from the server.  Do nothing
			break;
			
		case CLIENT_STATE_RUNNING:
		
			// Client is running; step the simulation
			stepSpace();
			
			// Send any manually-altered objects to the server
			sendAlteredObjects();
			
			// Call user run code
			runUser();
			break;
	}
}

void WiredMunkApp::resetAlteredState() {

	// Reset bodies
	for (int i = 0; i < _space->getBodies()->size(); ++i) {
		_space->getBodies()->at(i)->setAltered(false);
	}
	
	// Reset static bodies
	for (int i = 0; i < _space->getStaticBodies()->size(); ++i) {
		_space->getStaticBodies()->at(i)->setAltered(false);
	}
	
	// Reset altered shapes
	for (int i = 0; i < _space->getShapes()->size(); ++i) {
		_space->getShapes()->at(i)->setAltered(false);
	}
	
	// Reset static shapes
	for (int i = 0; i < _space->getStaticShapes()->size(); ++i) {
		_space->getStaticShapes()->at(i)->setAltered(false);
	}
	
	// Reset space
	_space->setAltered(false);
}

void WiredMunkApp::stepSpace() {
	
	// Calculate the time that has passed since the last time the
	// simulation ran
	struct timeval thisRunTime;
	struct timeval timeDiff;
	
	gettimeofday(&thisRunTime, NULL);
	
	// Get time difference between last time and this time
	timersub(&thisRunTime, &_lastRunTime, &timeDiff);
	
	// Calculate time in 85ths of a second
	int steps = (((cpFloat)timeDiff.tv_usec) / 1000000.0) * REFRESH_RATE;
	
	// Remember current run time if we are to step the simulation
	if (steps > 0) {
		struct timeval increaseTime;
		struct timeval nextRunTime;
		increaseTime.tv_usec = steps * (1000000.0 / REFRESH_RATE);
		timeradd(&_lastRunTime, &increaseTime, &nextRunTime);
		_lastRunTime = nextRunTime;
	}
	
	// Step the simulation
	cpFloat dt = 1.0/REFRESH_RATE;
	for (int i = 0; i < steps; ++i) {
		_space->step(dt);
		
		// Sample the simulation
		_sampler->sample(_space);
	}
}

void WiredMunkApp::sendAlteredObjects() {
	
	// Send any altered bodies
	for (int i = 0; i < _space->getBodies()->size(); ++i) {
		Body* body = _space->getBodies()->at(i);
		
		if (body->isAltered()) {
			body->sendObject();
		}
	}
	
	// Send any altered static bodies
	for (int i = 0; i < _space->getStaticBodies()->size(); ++i) {
		Body* body = _space->getStaticBodies()->at(i);
		
		if (body->isAltered()) {
			body->sendObject();
		}
	}
	
	// Send any altered shapes
	for (int i = 0; i < _space->getShapes()->size(); ++i) {
		Shape* shape = _space->getShapes()->at(i);
		
		if (shape->isAltered()) {
			shape->sendObject();
		}
	}
	
	// Send any altered static shapes
	for (int i = 0; i < _space->getStaticShapes()->size(); ++i) {
		Shape* shape = _space->getStaticShapes()->at(i);
		
		if (shape->isAltered()) {
			shape->sendObject();
		}
	}
}

Socket* WiredMunkApp::getSocket() {
	return &_socket;
}

void WiredMunkApp::requestHandshake() {
	
	// Send the request
	Message msg(Message::MESSAGE_HANDSHAKE, 0, NULL, this);
	_socket.sendMessage(&msg);
	
	// Move to next status
	_clientState = CLIENT_STATE_WAITING_HANDSHAKE;
	Debug::printf("Client switched to CLIENT_STATE_HANDSHAKE\n");
}
	
void WiredMunkApp::sendReady() {
	
	// Send the request
	Message msg(Message::MESSAGE_READY, 0, NULL);
	_socket.sendMessage(&msg);
	
	// Move to next status
	_clientState = CLIENT_STATE_WAITING_READY;
	Debug::printf("Client switched to CLIENT_STATE_WAITING_READY\n");
}	

void WiredMunkApp::handleResponseReceived(const Message& msg) { 
	switch (msg.getType()) {
		case Message::MESSAGE_HANDSHAKE:
		{
			// Extract data from the message
			const unsigned char* data = msg.getData();
			
			_clientId = SerialiseBase::deserialiseInt(data);
			data += SERIALISED_INT_SIZE;
			
			// Move to next status
			_clientState = CLIENT_STATE_WAITING_STARTUP;
			Debug::printf("Handshake received\n");
			Debug::printf("Client ID: %d\n", _clientId);
			Debug::printf("Client switched to CLIENT_STATE_WAITING_STARTUP\n");
			break;
		}
		case Message::MESSAGE_REJECT:
			
			// TODO: Do something useful here
			
			// Server full so client cannot participate
			break;
			
		default:
			break;
	}
}

void WiredMunkApp::handleMessageReceived(const Message& msg) { 
	switch (msg.getType()) {
			
		case Message::MESSAGE_STARTUP:
			
			// Move to the next status
			if (_clientState == CLIENT_STATE_WAITING_STARTUP) {
				_clientState = CLIENT_STATE_STARTING;
				Debug::printf("Client switched to CLIENT_STATE_STARTING\n");
			}
			break;
			
		case Message::MESSAGE_READY:
			
			// Move to the next status
			if (_clientState == CLIENT_STATE_WAITING_READY) {
				_clientState = CLIENT_STATE_RUNNING;
				Debug::printf("Client switched to CLIENT_STATE_RUNNING\n");
			}
			break;
			
		case Message::MESSAGE_SPACE:
			
			// Server has sent updated information on the simulation's space
			Debug::printf("Client received space data\n");
			_space->deserialise(msg.getData());
			break;
			
		default:
			break;
	}
}

void WiredMunkApp::sendSpace() {
	_space->sendObject();
}
