#ifndef _CLIENT_LIST_H_
#define _CLIENT_LIST_H_

#include <vector>
#include "client.h"

namespace WiredMunk {

	/**
	 * Class containing a list of clients connected to the server.
	 */
	class ClientList {
	public:
		
		/**
		 * Add a client to the list.
		 * @param client The client to add.
		 */
		void add(Client* client);
		
		/**
		 * Remove a client from the list.
		 * @param client The client to remove.
		 */
		void remove(Client* client);
		
		/**
		 * Check if the list contains the specified client.
		 * @param client The client.
		 */
		const bool contains(Client* client) const;
		
		/**
		 * Retrieve the client's socket address struct at the specified index.
		 * @param index Index within the client list of the struct to retrieve.
		 * @return The client.
		 */
		Client* at(const int index) const;
		
		/**
		 * Find the index of a client's socket address struct within the list.
		 * @param client The client to find.
		 * @return The index at which the pointer is located, or -1 if not
		 * found.
		 */
		const int find(Client* client) const;
		
		const Client* findByAddress(const struct sockaddr_in* address) const;
		
		/**
		 * Get the number of items in the list.
		 * @return The number of items in the list.
		 */
		const int size() const;
		
		/**
		 * Erase the contents of the list.
		 */
		void clear();
		
	private:
		std::vector<Client*> _clients;		/**< The list of client pointers */
	};
}

#endif
