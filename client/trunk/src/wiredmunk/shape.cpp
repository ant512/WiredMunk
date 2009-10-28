#include "shape.h"
#include "message.h"
#include "socket.h"
#include "wiredmunkapp.h"
#include "debug.h"

using namespace WiredMunk;

Shape::Shape(Body* body, cpFloat radius, cpVect offset) {
	_shape = cpCircleShapeNew(body->getBody(), radius, offset);
	_body = body;
}

Shape::Shape(Body* body, cpVect a, cpVect b, cpFloat radius) {
	_shape = cpSegmentShapeNew(body->getBody(), a, b, radius);
	_body = body;
}

Shape::Shape(Body* body, int numVerts, cpVect* verts, cpVect offset) {
	_shape = cpPolyShapeNew(body->getBody(), numVerts, verts, offset);
	_body = body;
}

Shape::Shape(BodyVector* bodyVector, BodyVector* staticBodyVector, const unsigned char* serialisedData) : NetworkObject(serialisedData) {
	_shape = NULL;
	_body = NULL;
	deserialise(bodyVector, staticBodyVector, serialisedData);
}

Shape::~Shape() {
	cpShapeFree(_shape);
}

BoundingBox Shape::cacheBoundingBox() {
	return BoundingBox(cpShapeCacheBB(_shape));
}

void Shape::resetShapeIdCounter() {
	cpResetShapeIdCounter();
}

BoundingBox Shape::getBoundingBox() {
	return BoundingBox(_shape->bb);
}

void Shape::setCollisionType(unsigned int type) {
	_shape->collision_type = type;
	
	setAltered(true);
}

void Shape::setCollisionGroup(unsigned int group) {
	_shape->group = group;
	
	setAltered(true);
}

void Shape::setCollisionLayers(unsigned int layers) {
	_shape->layers = layers;
	
	setAltered(true);
}

void Shape::setUserData(void* data) {
	_shape->data = data;
}

void Shape::setElasticity(cpFloat elasticity) {
	_shape->e = elasticity;
	
	setAltered(true);
}

void Shape::setFriction(cpFloat friction) {
	_shape->u = friction;
	
	setAltered(true);
}

unsigned int Shape::serialise(unsigned char* buffer) {
	
	// Ensure that the network object (containing unique ID) is the first item
	// serialised
	NetworkObject::serialise(buffer);
	buffer += NetworkObject::getSerialisedLength();
	
	// Serialise the body's ID into the buffer
	buffer += SerialiseBase::serialise(_body->getObjectId(), buffer);
	
	// Serialise the basic shape properties
	buffer += SerialiseBase::serialise(getElasticity(), buffer);
	buffer += SerialiseBase::serialise(getFriction(), buffer);
	buffer += SerialiseBase::serialise(getSurfaceVelocity(), buffer);
	buffer += SerialiseBase::serialise(getCollisionType(), buffer);
	buffer += SerialiseBase::serialise(getCollisionGroup(), buffer);
	buffer += SerialiseBase::serialise(getCollisionLayers(), buffer);
	
	// Serialise the shape type
	buffer += SerialiseBase::serialise((unsigned int)_shape->klass->type, buffer);
	
	// Serialise type-specific data
	switch (_shape->klass->type) {
		case CP_CIRCLE_SHAPE:
			
			buffer += SerialiseBase::serialise(((cpCircleShape*)_shape)->c, buffer);
			buffer += SerialiseBase::serialise(((cpCircleShape*)_shape)->r, buffer);
			buffer += SerialiseBase::serialise(((cpCircleShape*)_shape)->tc, buffer);
			break;
			
		case CP_SEGMENT_SHAPE:
			
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->a, buffer);
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->b, buffer);
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->n, buffer);
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->r, buffer);
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->ta, buffer);
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->tb, buffer);
			buffer += SerialiseBase::serialise(((cpSegmentShape*)_shape)->tn, buffer);
			break;
			
		case CP_POLY_SHAPE:
			
			// Vertexes and axes
			unsigned int numVerts = ((cpPolyShape*)_shape)->numVerts;
			buffer += SerialiseBase::serialise(numVerts, buffer);
			
			for (int i = 0; i < numVerts; ++i) {
				buffer += SerialiseBase::serialise(((cpPolyShape*)_shape)->verts[i], buffer);
				buffer += SerialiseBase::serialise(((cpPolyShape*)_shape)->axes[i].n, buffer);
				buffer += SerialiseBase::serialise(((cpPolyShape*)_shape)->axes[i].d, buffer);
				
				buffer += SerialiseBase::serialise(((cpPolyShape*)_shape)->tVerts[i], buffer);
				buffer += SerialiseBase::serialise(((cpPolyShape*)_shape)->tAxes[i].n, buffer);
				buffer += SerialiseBase::serialise(((cpPolyShape*)_shape)->tAxes[i].d, buffer);
			}
			
			break;
			
		case CP_NUM_SHAPES:
			
			// Undocumented!  Does not seem to be a way to create this shape
			// type in Chipmunk
			break;
	}
	
	return getSerialisedLength();
}

unsigned int Shape::deserialise(BodyVector* bodyVector, BodyVector* staticBodyVector, const unsigned char* data) {
	
	// Move past network object
	data += NetworkObject::getSerialisedLength();
	
	// Extract body ID
	int bodyId = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	// Wire up body pointer if it is not set
	if (_body == NULL) {
		
		bool isBody = false;
		
		// Locate the body in the body vector
		for (int i = 0; i < bodyVector->size(); ++i) {
			if (bodyVector->at(i)->getObjectId() == bodyId) {
				_body = bodyVector->at(i);
				isBody = true;
				break;
			}
		}
		
		// Did we find the body?
		if (!isBody) {
			
			// Body not found; must be a static body
			for (int i = 0; i < staticBodyVector->size(); ++i) {
				if (staticBodyVector->at(i)->getObjectId() == bodyId) {
					_body = staticBodyVector->at(i);
					break;
				}
			}
		}
	}
	
	// Extract basic shape data
	cpFloat elasticity = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	cpFloat friction = SerialiseBase::deserialiseDouble(data);
	data += SERIALISED_DOUBLE_SIZE;
	
	// Manually set surface velocity as it is not exposed
	cpVect surfaceVelocity = SerialiseBase::deserialiseVector(data);
	data += SERIALISED_VECTOR_SIZE;
	
	unsigned int collisionType = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	unsigned int collisionGroup = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	unsigned int collisionLayers = SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	// Deserialise the shape type
	cpShapeType type = (cpShapeType)SerialiseBase::deserialiseInt(data);
	data += SERIALISED_INT_SIZE;
	
	// Deserialise type-specific data
	switch (type) {
		case CP_CIRCLE_SHAPE:
		{
			cpVect centre = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			cpFloat radius = SerialiseBase::deserialiseDouble(data);
			data += SERIALISED_DOUBLE_SIZE;
			
			cpVect transformedCentre = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			if (_shape == NULL) {
				
				// Shape does not exist, so create new
				_shape = cpCircleShapeNew(_body->getBody(), radius, centre);
			} else {
				
				// Shape exists; override properties
				((cpCircleShape*)_shape)->r = radius;
				((cpCircleShape*)_shape)->c = centre;
			}
			
			// Override shape properties
			((cpCircleShape*)_shape)->tc = transformedCentre;
			
			break;
		}
		case CP_SEGMENT_SHAPE:
		{
			
			cpVect endPointA = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			cpVect endPointB = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			cpVect normal = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			cpFloat radius = SerialiseBase::deserialiseDouble(data);
			data += SERIALISED_DOUBLE_SIZE;
			
			cpVect transformedEndPointA = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			cpVect transformedEndPointB = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			cpVect transformedNormal = SerialiseBase::deserialiseVector(data);
			data += SERIALISED_VECTOR_SIZE;
			
			if (_shape == NULL) {
				
				// Shape does not exist, so create it
				_shape = cpSegmentShapeNew(_body->getBody(), endPointA, endPointB, radius);
			} else {
				
				// Override existing shape properties
				((cpSegmentShape*)_shape)->a = endPointA;
				((cpSegmentShape*)_shape)->b = endPointB;
				((cpSegmentShape*)_shape)->r = radius;
			}
			
			// Override remaining properties
			((cpSegmentShape*)_shape)->ta = transformedEndPointA;
			((cpSegmentShape*)_shape)->tb = transformedEndPointB;
			((cpSegmentShape*)_shape)->tn = transformedNormal;
			
			break;
		}
		case CP_POLY_SHAPE:
		{
			int numVerts = SerialiseBase::deserialiseInt(data);
			data += SERIALISED_INT_SIZE;
			
			// Allocate vertex and axes arrays
			cpVect* verts = new cpVect[numVerts];
			cpVect* transformedVerts = new cpVect[numVerts];
			cpPolyShapeAxis* axes = new cpPolyShapeAxis[numVerts];
			cpPolyShapeAxis* transformedAxes = new cpPolyShapeAxis[numVerts];
			
			for (int i = 0; i < numVerts; ++i) {
				verts[i] = SerialiseBase::deserialiseVector(data);
				data += SERIALISED_VECTOR_SIZE;
				
				axes[i].n = SerialiseBase::deserialiseVector(data);
				data += SERIALISED_VECTOR_SIZE;
				
				axes[i].d = SerialiseBase::deserialiseDouble(data);
				data += SERIALISED_DOUBLE_SIZE;
				
				transformedVerts[i] = SerialiseBase::deserialiseVector(data);
				data += SERIALISED_VECTOR_SIZE;
				
				transformedAxes[i].n = SerialiseBase::deserialiseVector(data);
				data += SERIALISED_VECTOR_SIZE;
				
				transformedAxes[i].d = SerialiseBase::deserialiseDouble(data);
				data += SERIALISED_DOUBLE_SIZE;
			}
			
			if (_shape == NULL) {
				
				// Shape does not exist, so create it
				cpVect offset;
				offset.x = 0;
				offset.y = 0;
				
				_shape = cpPolyShapeNew(_body->getBody(), numVerts, verts, offset);
			}
			
			// Override all verts and axes
			for (int i = 0; i < numVerts; ++i) {
				((cpPolyShape*)_shape)->verts[i] = verts[i];
				((cpPolyShape*)_shape)->axes[i] = axes[i];
				((cpPolyShape*)_shape)->tVerts[i] = transformedVerts[i];
				((cpPolyShape*)_shape)->tAxes[i] = transformedAxes[i];
			}
			
			// Clean up
			delete verts;
			delete axes;
			
			break;
			
		default:
			break;
		}
	}
	
	// Update common properties
	_shape->e = elasticity;
	_shape->u = friction;
	_shape->surface_v = surfaceVelocity;
	_shape->collision_type = collisionType;
	_shape->group = collisionGroup;
	_shape->layers = collisionLayers;
	
	// Remember that the shape matches the server
	setAltered(false);
	
	return getSerialisedLength();
}

unsigned int Shape::getSerialisedLength() {
	
	// Common shape data
	int size = NetworkObject::getSerialisedLength();
	size += SERIALISED_DOUBLE_SIZE * 2;
	size += SERIALISED_VECTOR_SIZE;
	size += SERIALISED_INT_SIZE * 5;
	
	// Type-specific shape data
	switch (_shape->klass->type) {
		case CP_CIRCLE_SHAPE:
			size += SERIALISED_VECTOR_SIZE * 2;
			size += SERIALISED_DOUBLE_SIZE;
			break;
			
		case CP_SEGMENT_SHAPE:
			size += SERIALISED_VECTOR_SIZE * 6;
			size += SERIALISED_DOUBLE_SIZE;
			break;
			
		case CP_POLY_SHAPE:
			size += SERIALISED_INT_SIZE;
			
			int numVerts = ((cpPolyShape*)_shape)->numVerts;
			
			size += numVerts * ((SERIALISED_VECTOR_SIZE * 4) + (SERIALISED_DOUBLE_SIZE * 2));
			break;
			
		case CP_NUM_SHAPES:
			// Not implemented, no docs!
			break;
	}
	
	return size;
}

void Shape::sendObject() {
	
	// Serialise the object
	int msgSize = getSerialisedLength();
	unsigned char msgData[msgSize];
	
	serialise(msgData);
	
	// Create a message
	Message msg(Message::MESSAGE_SHAPE, msgSize, msgData);
	
	// Send the message
	Socket* socket = WiredMunkApp::getApp()->getSocket();
	socket->sendMessage(&msg);
	
	// Remember that the changes have been transmitted
	setAltered(false);
	
	Debug::printf("Client transmitted shape data\n");
}
