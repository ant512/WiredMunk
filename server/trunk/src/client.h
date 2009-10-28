#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

namespace WiredMunk {
	
	/**
	 * Class containing information about a remote client.
	 */
	class Client {
	public:
		
		/**
		 * Constructor.
		 * @param address The client's address.
		 * @param clientId The client's ID.
		 */
		inline Client(const struct sockaddr_in* address, int clientId) {
			_address = *address;
			_id = clientId;
		};
		
		/**
		 * Destructor.
		 */
		inline ~Client() { };
		
		/**
		 * Get the client's address.
		 * @return The client's address.
		 */
		inline const struct sockaddr_in* getAddress() const {  return &_address; };
		
		/**
		 * Get the client's ID.
		 * @return The client's ID.
		 */
		inline const int getId() const { return _id; };
		
	private:
		struct sockaddr_in _address;				/**< The client's address */
		int _id;									/**< The client's ID */
	};
}

#endif
