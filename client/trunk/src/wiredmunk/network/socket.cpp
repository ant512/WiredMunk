#include <fcntl.h>
#include "socket.h"

using namespace WiredMunk;

Socket::~Socket() {
	shut();
}

bool Socket::open(const char* hostName, const int portNum) {
	
	// Uses code from http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#datagram
	// and http://www.linuxhowtos.org/data/6/client_udp.c
	
	struct hostent* hostEntry;
	
	// Attempt to open socket
	_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (_socket < 0) {
		perror("Cannot open socket");
		return false;
	}
	
	_server.sin_family = AF_INET;
	
	//Attempt to find host
	hostEntry = gethostbyname(hostName);
	if (hostEntry == 0) {
		perror("Cannot find host");
		return false;
	}
	
	bcopy((char*)hostEntry->h_addr, (char*)&_server.sin_addr, hostEntry->h_length);
	
	_server.sin_port = htons(portNum);

	// Ensure socket is non-blocking
	fcntl(_socket, F_SETFL, O_NONBLOCK);
	
	return true;
}

int Socket::poll() {
	
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
			
			// Attempt to treat message as a rely
			if (!handleReply(buffer, receivedBytes)) {
			
				// Not a reply; notify listeners of incoming data
				raiseMessageReceivedEvent(buffer, receivedBytes);
			}
			
			// Clean up
			delete buffer;
			
			return receivedBytes;
    }
}

bool Socket::write(const unsigned char* data, unsigned int length) const {
	int sentBytes = sendto(_socket, data, length, 0, (struct sockaddr*)&_server, sizeof(struct sockaddr_in));
	
	if (sentBytes == -1) {
		perror("Cannot write");
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

void Socket::raiseMessageReceivedEvent(unsigned char* data, int receivedBytes) const {
	
	// Construct a message
	Message msg(data);
	
	// Notify all handlers of the event
	for (unsigned int i = 0; i < _eventHandlers.size(); ++i) {
		_eventHandlers.at(i)->handleMessageReceived(msg);
	}
}

void Socket::sendMessage(const Message* msg) {
	
	int msgLength = msg->getFormattedMessageLength();
	unsigned char msgData[msgLength];
	
	msg->getFormattedMessage(msgData);
	
	write(msgData, msgLength);
	
	// Add a copy of the message to the pending list if it is expecting a
	// reply.
	if (msg->getResponseHandler() != NULL) {
		Message* newMsg = new Message(*msg);
		
		_responsePendingMessages.push_back(newMsg);
	}
}

bool Socket::handleReply(unsigned char* data, int receivedBytes) {
	
	// Construct a message object using the received data
	Message msg(data);
	
	Message* pendingMsg;
	
	// Loop through all sent messages that are awaiting responses and check
	// for a match
	for (int i = 0; i < _responsePendingMessages.size(); ++i) {
		pendingMsg = _responsePendingMessages.at(i);
		
		// Have we found the correct outbound message?
		if (pendingMsg->getId() == msg.getId()) {
			
			// Message found; call handler
			pendingMsg->getResponseHandler()->handleResponseReceived(msg);
			
			// Remove the message from the pending list
			delete pendingMsg;
			_responsePendingMessages.erase(_responsePendingMessages.begin() + i);
	
			return true;
		}
	}
	
	// No match found
	return false;
}
