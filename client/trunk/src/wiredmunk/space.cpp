#include "space.h"
#include "message.h"
#include "socket.h"
#include "wiredmunkapp.h"
#include "shape.h"
#include "body.h"
#include "joint.h"
#include "debug.h"

using namespace WiredMunk;

Space::Space() {
	_space = cpSpaceNew();
}

Space::Space(int iterations) {
	_space = cpSpaceNew();
	_space->iterations = iterations;
}

Space::Space(const unsigned char* serialisedData) : NetworkObject(serialisedData) {
	_space = NULL;
	
	deserialise(serialisedData);
}

unsigned int Space::deserialise(const unsigned char* data) {
	
	// Move past network object
	data += NetworkObject::getSerialisedLength();
	
	// Extract data from serialised form
	int iterations = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	cpVect gravity = SerialiseBase::deserialiseVector(data);
	data += SERIALISED_VECTOR_SIZE;
	
	cpFloat damping = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	if (_space == NULL) {
		
		// Space does not exist - create it
		_space = cpSpaceNew();
	}
	
	// Set space properties
	_space->iterations = iterations;
	_space->gravity = gravity;
	_space->damping = damping;
	
	// Deserialise bodies
	int bodies = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	for (int i = 0; i < bodies; ++i) {
		
		// Deserialise into a new body object
		Body* body = new Body(data);
		
		// Attempt to add the body to the body list
		if (!addBody(body)) {
			
			// Body already exists, so locate the body and deserialise into it
			for (int i = 0; i < _bodyList.size(); ++i) {
				if (_bodyList.at(i)->getObjectId() == body->getObjectId()) {
					_bodyList.at(i)->deserialise(data);
					
					// Move along data stream
					data += _bodyList.at(i)->getSerialisedLength();
					
					break;
				}
			}
			
			delete body;
		} else {
			
			// Move along data stream
			data += body->getSerialisedLength();
		}
	}
	
	// Deserialise static bodies
	int staticBodies = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	for (int i = 0; i < staticBodies; ++i) {
		
		// Deserialise into a new body object
		Body* body = new Body(data);
		
		// Attempt to add the body to the static body list
		if (!addStaticBody(body)) {
			
			// Body already exists, so locate the body and deserialise into it
			for (int i = 0; i < _staticBodyList.size(); ++i) {
				if (_staticBodyList.at(i)->getObjectId() == body->getObjectId()) {
					_staticBodyList.at(i)->deserialise(data);
					
					// Move along data stream
					data += _staticBodyList.at(i)->getSerialisedLength();
					
					break;
				}
			}
			
			delete body;
		} else {
			
			// Move along data stream
			data += body->getSerialisedLength();
		}
	}
	
	// Deserialise shapes
	int shapes = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	for (int i = 0; i < shapes; ++i) {
		
		// Deserialise into a new shape object
		Shape* shape = new Shape(&_bodyList, &_staticBodyList, data);
		
		// Attempt to add the shape to the shape list
		if (!addShape(shape)) {
			
			// Shape already exists, so locate the shape and deserialise into it
			for (int i = 0; i < _shapeList.size(); ++i) {
				if (_shapeList.at(i)->getObjectId() == shape->getObjectId()) {
					_shapeList.at(i)->deserialise(&_bodyList, &_staticBodyList, data);
					
					// Move along data stream
					data += _shapeList.at(i)->getSerialisedLength();
					
					break;
				}
			}
			
			delete shape;
		} else {
			
			// Move along data stream
			data += shape->getSerialisedLength();
		}
	}
	
	// Deserialise static shapes
	int staticShapes = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	for (int i = 0; i < staticShapes; ++i) {
		
		// Deserialise into a new shape object
		Shape* shape = new Shape(&_bodyList, &_staticBodyList, data);
		
		// Attempt to add the shape to the static shape list
		if (!addStaticShape(shape)) {
			
			// Shape already exists, so locate the shape and deserialise into it
			for (int i = 0; i < _staticShapeList.size(); ++i) {
				if (_staticShapeList.at(i)->getObjectId() == shape->getObjectId()) {
					_staticShapeList.at(i)->deserialise(&_bodyList, &_staticBodyList, data);
					
					// Move along data stream
					data += _staticShapeList.at(i)->getSerialisedLength();
					
					break;
				}
			}
			
			delete shape;
		} else {
			
			// Move along data stream
			data += shape->getSerialisedLength();
		}
	}
	
	/*
	// Deserialise joints
	int joints = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	for (int i = 0; i < joints; ++i) {
		addJoint(new Joint(data));
		data += _jointList.at(_jointList.size() - 1)->getSerialisedLength();
	}
	 */
	
	// Remember that the body matches the server
	setAltered(false);
	
	return getSerialisedLength();
}

Space::~Space() {
	cpSpaceFreeChildren(_space);
	
	cpSpaceFree(_space);
	_bodyList.clear();
	_shapeList.clear();
	_staticShapeList.clear();
	_jointList.clear();
}

bool Space::addShape(Shape* shape) {
	
	// Ensure this shape does not exist
	for (int i = 0; i < _shapeList.size(); ++i) {
		if (_shapeList.at(i)->getObjectId() == shape->getObjectId()) return false;
	}
	
	// Shape does not exist, so add shape
	cpSpaceAddShape(_space, shape->getShape());
	_shapeList.push_back(shape);
	
	return true;
}

bool Space::addStaticShape(Shape* shape) {
	
	// Ensure this shape does not exist
	for (int i = 0; i < _staticShapeList.size(); ++i) {
		if (_staticShapeList.at(i)->getObjectId() == shape->getObjectId()) return false;
	}
	
	// Shape does not exist, so add shape
	cpSpaceAddStaticShape(_space, shape->getShape());
	_staticShapeList.push_back(shape);
	
	return true;
}

bool Space::addBody(Body* body) {
	
	// Ensure this body does not exist
	for (int i = 0; i < _bodyList.size(); ++i) {
		if (_bodyList.at(i)->getObjectId() == body->getObjectId()) return false;
	}
	
	// Body does not exist, so add body
	cpSpaceAddBody(_space, body->getBody());
	_bodyList.push_back(body);
	
	return true;
}

bool Space::addStaticBody(Body* body) {
	
	// Ensure this body does not exist
	for (int i = 0; i < _staticBodyList.size(); ++i) {
		if (_staticBodyList.at(i)->getObjectId() == body->getObjectId()) return false;
	}
	
	// Body does not exist, so add body.  Note that the body is not added to
	// Chipmunk's data structures - only the wrapper keeps track of these in
	// a separate list, for serialisation purposes
	_staticBodyList.push_back(body);
	
	return true;
}

void Space::addJoint(Joint* joint) {
	cpSpaceAddJoint(_space, joint->getJoint());
	_jointList.push_back(joint);
}

void Space::removeShape(Shape* shape) {
	cpSpaceRemoveShape(_space, shape->getShape());
	
	for (int i = 0; i < _shapeList.size(); ++i) {
		if (_shapeList.at(i) == shape) {
			_shapeList.erase(_shapeList.begin() + i);
		}
	}
}

void Space::removeStaticShape(Shape* shape) {
	cpSpaceRemoveStaticShape(_space, shape->getShape());
	
	for (int i = 0; i < _staticShapeList.size(); ++i) {
		if (_staticShapeList.at(i) == shape) {
			_staticShapeList.erase(_staticShapeList.begin() + i);
		}
	}
}

void Space::removeBody(Body* body) {
	cpSpaceRemoveBody(_space, body->getBody());
	
	for (int i = 0; i < _bodyList.size(); ++i) {
		if (_bodyList.at(i) == body) {
			_bodyList.erase(_bodyList.begin() + i);
		}
	}
}

void Space::removeJoint(Joint* joint) {
	cpSpaceRemoveJoint(_space, joint->getJoint());
	
	for (int i = 0; i < _jointList.size(); ++i) {
		if (_jointList.at(i) == joint) {
			_jointList.erase(_jointList.begin() + i);
		}
	}
}

void Space::setIterations(int iterations) {
	_space->iterations = iterations;
}

void Space::setGravity(cpVect gravity) {
	_space->gravity = gravity;
}

void Space::setDamping(cpFloat damping) {
	_space->damping = damping;
}

void Space::resizeStaticHash(cpFloat dim, int count) {
	cpSpaceResizeStaticHash(_space, dim, count);
}

void Space::resizeActiveHash(cpFloat dim, int count) {
	cpSpaceResizeActiveHash(_space, dim, count);
}

void Space::rehashStatic() {
	cpSpaceRehashStatic(_space);
}

void Space::step(cpFloat dt) {
	cpSpaceStep(_space, dt);
}

unsigned int Space::serialise(unsigned char* buffer) {
	
	unsigned char* oldBuffer = buffer;
	
	// Ensure that the network object (containing unique ID) is the first item
	// serialised
	buffer += NetworkObject::serialise(buffer);

	// Serialise basic properties
	buffer += SerialiseBase::serialise((unsigned int)getIterations(), buffer);
	buffer += SerialiseBase::serialise(getGravity(), buffer);
	buffer += SerialiseBase::serialise(getDamping(), buffer);
	
	// Bodies
	buffer += SerialiseBase::serialise((unsigned int)_bodyList.size(), buffer);

	for (int i = 0; i < _bodyList.size(); ++i) {
		_bodyList.at(i)->serialise(buffer);
		buffer += _bodyList.at(i)->getSerialisedLength();
	}
	
	// Static bodies
	buffer += SerialiseBase::serialise((unsigned int)_staticBodyList.size(), buffer);
	
	for (int i = 0; i < _staticBodyList.size(); ++i) {
		_staticBodyList.at(i)->serialise(buffer);
		buffer += _staticBodyList.at(i)->getSerialisedLength();
	}
	
	// Shapes
	buffer += SerialiseBase::serialise((unsigned int)_shapeList.size(), buffer);

	for (int i = 0; i < _shapeList.size(); ++i) {
		_shapeList.at(i)->serialise(buffer);
		buffer += _shapeList.at(i)->getSerialisedLength();
	}
	
	// Static shapes
	buffer += SerialiseBase::serialise((unsigned int)_staticShapeList.size(), buffer);

	for (int i = 0; i < _staticShapeList.size(); ++i) {
		_staticShapeList.at(i)->serialise(buffer);
		buffer += _staticShapeList.at(i)->getSerialisedLength();
	}
	
	// Joints
	buffer += SerialiseBase::serialise((unsigned int)_jointList.size(), buffer);
	
	/*
	for (int i = 0; i < _jointList.size(); ++i) {
		_jointList.at(i)->serialise(buffer);
		buffer += _jointList.at(i)->getSerialisedLength();
	}
	 */
	
	return buffer - oldBuffer;
}

unsigned int Space::getSerialisedLength() {
	int size = NetworkObject::getSerialisedLength();
	size += SERIALISED_INT_SIZE * 6;
	size += SERIALISED_VECTOR_SIZE;
	size += SERIALISED_DOUBLE_SIZE;
	
	for (int i = 0; i < _bodyList.size(); ++i) {
		size += _bodyList.at(i)->getSerialisedLength();
	}
	
	for (int i = 0; i < _staticBodyList.size(); ++i) {
		size += _staticBodyList.at(i)->getSerialisedLength();
	}
	
	for (int i = 0; i < _staticShapeList.size(); ++i) {
		size += _staticShapeList.at(i)->getSerialisedLength();
	}
	
	for (int i = 0; i < _shapeList.size(); ++i) {
		size += _shapeList.at(i)->getSerialisedLength();
	}
	
	for (int i = 0; i < _jointList.size(); ++i) {
		size += _jointList.at(i)->getSerialisedLength();
	}
	
	return size;
}

void Space::sendObject() {
	
	// Serialise the object
	int msgSize = getSerialisedLength();
	unsigned char msgData[msgSize];
	
	serialise(msgData);
	
	// Create a message
	Message msg(Message::MESSAGE_SPACE, msgSize, msgData);
	
	// Send the message
	Socket* socket = WiredMunkApp::getApp()->getSocket();
	socket->sendMessage(&msg);
	
	// Remember that the changes have been transmitted
	setAltered(false);
	
	Debug::printf("Client transmitted space data\n");
}
