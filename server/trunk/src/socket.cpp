#include <fcntl.h>
#include "socket.h"
#include "debug.h"

using namespace WiredMunk;

bool Socket::open(const int portNum) {
	
	// Uses code from http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#datagram
	// and http://www.linuxhowtos.org/data/6/server_udp.c
	
	struct sockaddr_in server;
	int length;
	
	_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (_socket < 0) {
		perror("Error opening socket");
		return false;
	}
	
	length = sizeof(server);
	bzero(&server, length);
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portNum);
	
	if (bind(_socket, (struct sockaddr*)&server, length) < 0) {
		perror("Error binding socket");
		return false;
	}
	
	// Ensure socket is non-blocking
	fcntl(_socket, F_SETFL, O_NONBLOCK);
	
    return true;
}

Socket::~Socket() {
	shut();
}

int Socket::poll() const {

	unsigned char* buffer;
	int receivedBytes;
	struct sockaddr_in remoteAddress;
	socklen_t addressLen;
	
	buffer = new unsigned char[MESSAGE_BUFFER_LENGTH];
	
	addressLen = sizeof(remoteAddress);
	
	// Poll socket for data
	receivedBytes = recvfrom(_socket, buffer, MESSAGE_BUFFER_LENGTH - 1 , 0, (struct sockaddr *)&remoteAddress, &addressLen);
	
	// Ensure buffer terminates correctly
	buffer[MESSAGE_BUFFER_LENGTH - 1] = '\0';
	
	// Check received data
	switch (receivedBytes) {
		case -1:
			// No data received
			delete buffer;
			return 0;
		case 0:
			// Socket closed
			// TODO: Handle this gracefully instead of ignoring it
			delete buffer;
			return 0;
		default:
			// Data received
			
			// Does the data have the WiredMunk header?
			if (strncmp(MESSAGE_HEADER, (char*)buffer, 4) != 0) {
				
				// Not a valid message - discard it
				delete buffer;
				return 0;
			}
			
			// Valid message received
			Debug::printf("Received incoming message\n");
			
			// Notify listeners of incoming data
			raiseMessageReceivedEvent(&remoteAddress, buffer, receivedBytes);
			
			// Clean up
			delete buffer;
			
			return receivedBytes;
    }
}

bool Socket::write(const unsigned char* data, unsigned int length, const struct sockaddr_in* address) const {
	int sentBytes = sendto(_socket, data, length, 0, (struct sockaddr*)address, sizeof(struct sockaddr_in));
	
	if (sentBytes == -1) {
		perror("Error writing to socket");
		return false;
	}
	
	return true;
}

void Socket::shut() {
	shutdown(_socket, 2);
	close(_socket);
}

void Socket::addSocketEventHandler(SocketEventHandler* handler) {
	_eventHandlers.push_back(handler);
}

void Socket::removeSocketEventHandler(SocketEventHandler* handler) {
	for (unsigned int i = 0; i < _eventHandlers.size(); ++i) {
		if (_eventHandlers.at(i) == handler) {
			_eventHandlers.erase(_eventHandlers.begin() + i);
			return;
		}
	}
}

void Socket::raiseMessageReceivedEvent(const struct sockaddr_in* address, unsigned char* data, int receivedBytes) const {
	
	// Construct a message
	Message msg(data, address);
	
	// Notify all handlers of the event
	for (unsigned int i = 0; i < _eventHandlers.size(); ++i) {
		_eventHandlers.at(i)->handleMessageReceived(msg);
	}
}

void Socket::sendMessage(const Message* msg) const {
	
	int msgLength = msg->getFormattedMessageLength();
	unsigned char msgData[msgLength];
	
	msg->getFormattedMessage(msgData);
	
	write(msgData, msgLength, msg->getAddress());
}
