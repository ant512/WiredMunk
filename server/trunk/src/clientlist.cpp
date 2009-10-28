#include "clientlist.h"

using namespace WiredMunk;

void ClientList::add(Client* client) {
	if (!contains(client)) {
		_clients.push_back(client);
	}
}

void ClientList::remove(Client* client) {
	int index = find(client);
	
	if (index > -1) {
		_clients.erase(_clients.begin() + index);
	}
}

const bool ClientList::contains(Client* client) const {
	return (find(client) > -1);
}

Client* ClientList::at(const int index) const {
	return _clients.at(index);
}

const int ClientList::find(Client* client) const {
	for (int i = 0; i < _clients.size(); ++i) {
		if (_clients.at(i) == client) {
			return i;
		}
	}
	
	return -1;
}

const Client* ClientList::findByAddress(const struct sockaddr_in* address) const {
	const struct sockaddr_in* currentAddress;
	
	for (int i = 0; i < _clients.size(); ++i) {
		currentAddress = _clients.at(i)->getAddress();
		
		// Compare sockaddr_in structure data
		if (currentAddress->sin_len != address->sin_len) continue;
		if (currentAddress->sin_family != address->sin_family) continue;
		if (currentAddress->sin_port != address->sin_port) continue;
		if (currentAddress->sin_addr.s_addr != address->sin_addr.s_addr) continue;
		
		// Found a match
		return _clients.at(i);
	}

	// No match
	return NULL;
}

const int ClientList::size() const {
	return _clients.size();
}

void ClientList::clear() {
	for (int i = 0; i < _clients.size(); ++i) {
		delete _clients.at(i);
	}
	
	_clients.clear();
}
