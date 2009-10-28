#include "server.h"
#include "debug.h"

using namespace WiredMunk;

Server* Server::_singleton = NULL;

Server::Server(int clientCount, int portNum) {
	
	_socket = new Socket();
	_clientManager = new ClientManager(_socket, clientCount);
	_singleton = this;
	
	_socket->addSocketEventHandler(_clientManager);
	_socket->open(portNum);
	
	_simulation = new Simulation();
	_socket->addSocketEventHandler(_simulation);
	
	Debug::printf("Server started\n");
	Debug::printf("Port:    %d\n", portNum);
	Debug::printf("Clients: %d\n", clientCount);
}

Server::~Server() {
	_socket->shut();
	
	delete _clientManager;
	delete _socket;
	delete _simulation;
	
	Debug::printf("Server stopped\n");
}

void Server::run() {
	while(1) {
		_socket->poll();
		
		_simulation->run();
	}
}
