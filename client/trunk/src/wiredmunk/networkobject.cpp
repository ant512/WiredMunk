#include "networkobject.h"
#include "socket.h"
#include "wiredmunkapp.h"
#include "message.h"

#include <stdio.h>

using namespace WiredMunk;

unsigned int NetworkObject::_nextId = 0;

NetworkObject::NetworkObject() {
	_objectId = getNextId();
	_isAltered = false;
}

NetworkObject::NetworkObject(const unsigned char* serialisedData) {
	_isAltered = false;
	deserialise(serialisedData);
}

void NetworkObject::requestObjectId() {
	Socket* socket = WiredMunkApp::getApp()->getSocket();
	
	Message msg(Message::MESSAGE_OBJECT_ID, 0, NULL, this);
	
	socket->sendMessage(&msg);
}

void NetworkObject::handleResponseReceived(const Message& msg) {
	
	switch (msg.getType()) {
		case Message::MESSAGE_OBJECT_ID:
			
			// Received an object ID - extract it
			_objectId = SerialiseBase::deserialiseInt(msg.getData());
			
			printf("Received object ID: %d\n", _objectId);
			
			// Transmit the object across the network
			sendObject();
			
			break;
			
		default:
			break;
	}
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
