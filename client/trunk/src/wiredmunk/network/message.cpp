#include "message.h"

#include <cstring>

using namespace WiredMunk;

unsigned short Message::_nextId = 1;

Message::Message(MessageType type, unsigned short dataLength, const unsigned char* data, SocketEventHandler* responseHandler) {
	_type = type;
	_responseHandler = responseHandler;
	_data = NULL;
	
	_id = getNextId();
	
	setData(data, dataLength);
}

Message::Message(const unsigned char* data, SocketEventHandler* responseHandler) {
	
	_responseHandler = responseHandler;
	
	_type = (MessageType)data[4];				// 1 byte type
	_dataLength = (data[5] << 8) | data[6];		// 2 byte length
	_id = (data[7] << 8) | data[8];				// 2 byte id number
	
	if (_dataLength > 0) {
		_data = new unsigned char[_dataLength];
		memcpy(_data, data + MESSAGE_HEADER_LENGTH, _dataLength);
	} else {
		_data = NULL;
	}
}

Message::Message(Message const& copy) {
	
	_responseHandler = copy.getResponseHandler();
	_id = copy.getId();
	_type = copy.getType();

	setData(copy.getData(), copy.getDataLength());
}

Message::~Message() {
	if (_data != NULL) {
		delete _data;
	}
}

unsigned int Message::getFormattedMessage(unsigned char* buffer) const {

	int messageLen = getFormattedMessageLength();
	
	// Build message header
	memcpy(buffer, MESSAGE_HEADER, MESSAGE_HEADER_LENGTH);				// 4 byte identifier
	buffer[4] = (char)_type;					// 1 byte type
	buffer[5] = (char)(_dataLength >> 8);		// 1st byte of length
	buffer[6] = (char)(_dataLength & 0xFF);		// 2nd byte of length
	buffer[7] = (char)(_id >> 8);				// 1st byte of id number
	buffer[8] = (char)(_id & 0xFF);				// 2nd byte of id number
	
	if (_dataLength > 0) {
		memcpy(buffer + MESSAGE_HEADER_LENGTH, _data, _dataLength);		// n byte data
	}
	
	return messageLen;
}

void Message::setData(const unsigned char* data, unsigned short dataLength) {
	_dataLength = dataLength;
	
	if (_dataLength > 0) {
		_data = new unsigned char[_dataLength];
		memcpy(_data, data, _dataLength);
	} else {
		_data = NULL;
	}
}

unsigned int Message::getFormattedMessageLength() const {
	return MESSAGE_HEADER_LENGTH + _dataLength;
}
