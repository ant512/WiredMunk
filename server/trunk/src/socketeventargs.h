#ifndef _SOCKET_EVENT_ARGS_H_
#define _SOCKET_EVENT_ARGS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

namespace WiredMunk {
	
	/**
	 * Class containing data associated with Socket class events.
	 */
	class SocketEventArgs {
	public:
		
		/**
		 * Constructor.
		 * @param address Address of the client that caused the event.
		 * @param data Data associated with the event.
		 * @param dataLength Length of the data.
		 */
		SocketEventArgs(const struct sockaddr_in* address, const char* data, int dataLength) {
			_address = address;
			_data = data;
			_dataLength = dataLength;
		};
		
		/**
		 * Get the address of the remote client associated with the event.
		 * @return The address of the client.
		 */
		const struct sockaddr_in* getAddress() const { return _address; };
		
		/**
		 * Get the event data.
		 * @return The event data.
		 */
		const char* getData() const { return _data; };
		
		/**
		 * Get the length of the event data.
		 * @return The length of the event data.
		 */
		int getDataLength() const { return _dataLength; };
		
	private:
		const struct sockaddr_in* _address;		/**< The remote address */
		const char* _data;						/**< The event data */
		int _dataLength;						/**< The data length */
	};
}

#endif
