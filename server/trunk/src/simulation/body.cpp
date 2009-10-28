#include "body.h"
#include "message.h"
#include "socket.h"
#include "simulation.h"
#include "server.h"

using namespace WiredMunk;

Body::Body() {
	Body(1.0f, 1.0f);
}

Body::Body(cpFloat mass, cpFloat moment) : NetworkObject() {
	_body = cpBodyNew(mass,  moment);
}

Body::Body(const unsigned char* serialisedData) : NetworkObject(serialisedData) {
	_body = NULL;
	
	deserialise(serialisedData);
}

Body::~Body() {
	cpBodyFree(_body);
}

void Body::setMass(cpFloat mass) {
	cpBodySetMass(_body, mass);
}

void Body::setMoment(cpFloat moment) {
	cpBodySetMoment(_body, moment);
}

void Body::setAngle(cpFloat angle) {
	cpBodySetAngle(_body, angle);
} 

void Body::setPosition(cpVect position) {
	_body->p = position;
}

void Body::setVelocity(cpVect velocity) {
	_body->v = velocity;
}

void Body::setAngularVelocity(cpFloat velocity) {
	_body->w = velocity;
}

void Body::setTorque(cpFloat torque) {
	_body->t = torque;
}	

void Body::slew(cpVect position, cpFloat dt) {
	cpBodySlew(_body, position, dt);
}

void Body::updateVelocity(cpVect gravity, cpFloat damping, cpFloat dt) {
	cpBodyUpdateVelocity(_body, gravity, damping, dt);
}

void Body::updatePosition(cpFloat dt) {
	cpBodyUpdatePosition(_body, dt);
}

cpVect Body::localToWorld(cpVect vector) {
	return cpBodyLocal2World(_body, vector);
}

cpVect Body::worldToLocal(cpVect vector) {
	return cpBodyWorld2Local(_body, vector);
}

void Body::resetForces() {
	cpBodyResetForces(_body);
}

void Body::applyImpulse(cpVect j, cpVect r) {
	cpBodyApplyImpulse(_body, j, r);
}

void Body::applyForce(cpVect f, cpVect r) {
	cpBodyApplyForce(_body, f, r);
}

void Body::applyTorque(cpFloat torque) {
	_body->t += torque;
}

unsigned int Body::serialise(unsigned char* buffer) {
	
	// Ensure that the network object (containing unique ID) is the first item
	// serialised
	NetworkObject::serialise(buffer);
	buffer += NetworkObject::getSerialisedLength();
	
	buffer += SerialiseBase::serialise(getMass(), buffer);
	buffer += SerialiseBase::serialise(getMoment(), buffer);
	buffer += SerialiseBase::serialise(getPosition(), buffer);
	buffer += SerialiseBase::serialise(getVelocity(), buffer);
	buffer += SerialiseBase::serialise(getForce(), buffer);
	buffer += SerialiseBase::serialise(getAngle(), buffer);
	buffer += SerialiseBase::serialise(getAngularVelocity(), buffer);
	buffer += SerialiseBase::serialise(getTorque(), buffer);
	
	return getSerialisedLength();
}

unsigned int Body::getSerialisedLength() {
	int size = NetworkObject::getSerialisedLength();
	size += SERIALISED_DOUBLE_SIZE * 5;
	size += SERIALISED_VECTOR_SIZE * 3;
	
	return size;
}

unsigned int Body::deserialise(const unsigned char* data) {
	
	// Move past network object
	data += NetworkObject::getSerialisedLength();
	
	// Extract data from serialised form
	cpFloat mass = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	cpFloat moment = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	cpVect position = SerialiseBase::deserialiseVector(data);
	data += SERIALISED_VECTOR_SIZE;
	
	cpVect velocity = SerialiseBase::deserialiseVector(data);
	data += SERIALISED_VECTOR_SIZE;
	
	cpVect force = SerialiseBase::deserialiseVector(data);
	data += SERIALISED_VECTOR_SIZE;
	
	cpFloat angle = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	cpFloat angularVelocity = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	cpFloat torque = SerialiseBase::deserialiseDouble(data);
	
	// Update body
	if (_body == NULL) {
		
		// Body does not exist, so create
		_body = cpBodyNew(mass, moment);
	} else {
		
		// Update existing body
		cpBodySetMass(_body, mass);
		cpBodySetMoment(_body, moment);
	}
	
	_body->p = position;
	_body->v = velocity;
	_body->f = force;
	_body->t = torque;
	
	cpBodySetAngle(_body, angle);
	_body->w = angularVelocity;
	
	return getSerialisedLength();
}

void Body::sendObject(const struct sockaddr_in* address) {
	
	// Serialise the object
	int msgSize = getSerialisedLength();
	unsigned char msgData[msgSize];
	
	serialise(msgData);
	
	// Create a message
	Message msg(Message::MESSAGE_BODY, 0, msgSize, msgData, address);
	
	// Send the message
	Socket* socket = Server::getServer()->getSocket();
	socket->sendMessage(&msg);
}
