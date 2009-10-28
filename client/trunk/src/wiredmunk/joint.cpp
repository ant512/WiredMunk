#include "joint.h"

using namespace WiredMunk;

Joint::Joint(Body* body1, Body* body2, cpVect anchor1, cpVect anchor2) : NetworkObject() {
	_joint = cpPinJointNew(body1->getBody(), body2->getBody(), anchor1, anchor2);
}

Joint::Joint(Body* body1, Body* body2, cpVect anchor1, cpVect anchor2, cpFloat min, cpFloat max) : NetworkObject() {
	_joint = cpSlideJointNew(body1->getBody(), body2->getBody(), anchor1, anchor2, min, max);
}

Joint::Joint(Body* body1, Body* body2, cpVect pivot) : NetworkObject() {
	_joint = cpPivotJointNew(body1->getBody(), body2->getBody(), pivot);
}

Joint::Joint(Body* body1, Body* body2, cpVect grooveA, cpVect grooveB, cpVect anchor) : NetworkObject() {
	_joint = cpGrooveJointNew(body1->getBody(), body2->getBody(), grooveA, grooveB, anchor);
}

Joint::Joint(const unsigned char* serialisedData) : NetworkObject(serialisedData) {
	// TODO: Fill this method
}

Joint::~Joint() {
	cpJointFree(_joint);
}

cpJoint* Joint::getJoint() {
	return _joint;
}

unsigned int Joint::serialise(unsigned char* buffer) {
	// TODO: Fill this method
	
	return getSerialisedLength();
}

unsigned int Joint::deserialise(const unsigned char* data) {
	// TODO: Fill this method
	
	return getSerialisedLength();
}

unsigned int Joint::getSerialisedLength() {
	
	// TODO: Replace with correct value
	return 0;
}
