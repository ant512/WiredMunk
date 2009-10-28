#include "networkobject.h"
#include "socket.h"
#include "simulation.h"
#include "message.h"

#include <stdio.h>

using namespace WiredMunk;

unsigned int NetworkObject::_nextId = 0;

NetworkObject::NetworkObject() {
	_objectId = getNextId();
}

NetworkObject::NetworkObject(const unsigned char* serialisedData) {
	deserialise(serialisedData);
}

unsigned int NetworkObject::serialise(unsigned char* buffer) {
	return SerialiseBase::serialise(_objectId, buffer);
}

unsigned int NetworkObject::deserialise(const unsigned char* data) {
	_objectId = SerialiseBase::deserialiseInt(data);
	return SERIALISED_INT_SIZE;
}

unsigned int NetworkObject::getSerialisedLength() {
	return SERIALISED_INT_SIZE;
}

void NetworkObject::setObjectId(unsigned int objectId) {
	_objectId = objectId;
}
