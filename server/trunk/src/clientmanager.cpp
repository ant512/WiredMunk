#include "clientmanager.h"
#include "client.h"
#include "debug.h"
#include "idserver.h"

using namespace WiredMunk;

ClientManager::ClientManager(Socket* socket, int clientCount) {
	_socket = socket;
	_clientCount = clientCount;
}

void ClientManager::handleMessageReceived(const Message& msg) {
	
	switch (msg.getType()) {
			
		case Message::MESSAGE_HANDSHAKE:
			handleHandshakeReceived(msg);
			break;
		
		case Message::MESSAGE_READY:
			handleReadyReceived(msg);
			break;
			
		case Message::MESSAGE_OBJECT_ID:
			handleObjectIdRequestReceived(msg);
			break;
			
		default: 
			break;
	}
}

void ClientManager::handleHandshakeReceived(const Message& msg) {
	
	// Client trying to connect
	Debug::printf("Client requests handshake\n");
	
	// Attempt to add the client to the list; existing clients are ignored
	addClient(msg.getAddress());
	
	// Attempt to find the client in the list.  If the client exists,
	// the client has been added to the pool of participants and we can
	// send it a handshake response and its ID.  If not, the client has
	// not been added as a participant and must be sent a rejection
	// message instead
	const Client* client = _clients.findByAddress(msg.getAddress());
	
	if (client != NULL) {
		
		// Send acknowledgement - this happens regardless of whether or
		// not the client was added, as a client may request a handshake
		// multiple times if the reply packet gets lost
		
		// Store the client ID in a string			
		int clientId = client->getId();
		
		unsigned char data[4];
		data[0] = (unsigned char)(clientId >> 24);		// 1st byte of ID
		data[1] = (unsigned char)(clientId >> 16);		// 2nd byte of ID
		data[2] = (unsigned char)(clientId >> 8);		// 3rd byte of ID
		data[3] = (unsigned char)(clientId & 0xFF);		// 4th byte of ID
		
		Message reply(Message::MESSAGE_HANDSHAKE, msg.getId(), 5, data, msg.getAddress());
		_socket->sendMessage(&reply);
		
		// Do we have enough clients to start the simulation?
		if (_clients.size() == _clientCount) {
			
			// Send start message to all clients
			Message startMessage(Message::MESSAGE_STARTUP, 0, 0, NULL, msg.getAddress());
			for (int i = 0; i < _clients.size(); ++i) {
				startMessage.setAddress(_clients.at(i)->getAddress());
				_socket->sendMessage(&startMessage);
			}
		}
	} else {
		
		// Send rejection message
		Message reply(Message::MESSAGE_REJECT, msg.getId(), 0, NULL, msg.getAddress());
		_socket->sendMessage(&reply);
	}
}

void ClientManager::handleReadyReceived(const Message& msg) {

	// Client is ready to start
	_readyClientCount++;
	
	// Are all clients ready?
	if (_readyClientCount == _clientCount) {
		
		// Send commencement message to all clients
		Message reply(Message::MESSAGE_READY, 0, 0, NULL, msg.getAddress());
		for (int i = 0; i < _clients.size(); ++i) {
			reply.setAddress(_clients.at(i)->getAddress());
			_socket->sendMessage(&reply);
		}
	}
}

void ClientManager::handleObjectIdRequestReceived(const Message& msg) {

	// Client requesting a unique object ID
	
	// Store the object ID in a string			
	int objectId = IDServer::getNextNetworkObjectId();
	
	unsigned char data[4];
	data[0] = (unsigned char)(objectId >> 24);
	data[1] = (unsigned char)(objectId >> 16);
	data[2] = (unsigned char)(objectId >> 8);
	data[3] = (unsigned char)(objectId & 0xFF);
	
	// Send reply
	Message reply(msg.getType(), msg.getId(), 4, data, msg.getAddress());
	_socket->sendMessage(&reply);
}

void ClientManager::addClient(const struct sockaddr_in* address) {

	// Only add client if it does not already exist
	if (_clients.findByAddress(address) != NULL) return;
			
	// Only add client if we do not have enough clients yet
	if (_clientCount <= _clients.size()) return;
	
	// Add the new client
	Client* client = new Client(address, IDServer::getNextClientId());
	_clients.add(client);
}

void ClientManager::sendSpace(Space* space) {
	for (int i = 0; i < _clients.size(); ++i) {
		space->sendObject(_clients.at(i)->getAddress());
	}
}
