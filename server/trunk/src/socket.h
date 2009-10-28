#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <vector>
#include "socketeventhandler.h"
#include "socketeventargs.h"
#include "message.h"

#define MESSAGE_BUFFER_LENGTH 16384

namespace WiredMunk {
	
	/**
	 * Represents a socket that can be opened to listen for incoming messages.
	 * Socket is bidirectional and can send messages as well as receive them.
	 * Uses UDP datagrams for communication.  All calls are non-blocking.
	 */
	class Socket {
	public:
		
		/**
		 * Constructor.
		 */
		inline Socket() { };
		
		/**
		 * Open a connection.
		 * @param portNum Port number of the computer to connect to.
		 * @return True if the socket connected successfully.
		 */
		bool open(const int portNum);
		
		/**
		 * Check for incoming data from socket.  Raises a message received event
		 * if any messages are found.
		 * @return The number of bytes received.
		 */
		int poll() const;
		
		/**
		 * Close the socket.
		 */
		void shut();
		
		/**
		 * Destructor.
		 */
		~Socket();
		
		/**
		 * Add an event handler to listen for socket events.
		 * @param handler Event handler to add to list.
		 */
		void addSocketEventHandler(SocketEventHandler* handler);
		
		/**
		 * Remove an event handler from the list of event handlers.
		 * @param handler Event handler to remove from list.
		 */
		void removeSocketEventHandler(SocketEventHandler* handler);
		
		/**
		 * Sends the message.
		 * @param msg Message to send.
		 */
		void sendMessage(const Message* msg) const;
		
	private:
		int _socket;										/**< File descriptor of socket */
		std::vector<SocketEventHandler*> _eventHandlers;	/**< List of event handlers */
		
		/**
		 * Write data to the socket.
		 * @param data Data to send.
		 * @param length Length of the data.
		 * @param address Address to write to.
		 * @return True if data was sent; false if not.
		 */
		bool write(const unsigned char* data, unsigned int length, const struct sockaddr_in* address) const;
		
		/**
		 * Raise a message received event to all event handlers.
		 * @param address Address of remote client that sent the message.
		 * @param data Data received in message.
		 * @param receivedBytes Number of bytes received.
		 */
		void raiseMessageReceivedEvent(const struct sockaddr_in* address, unsigned char* data, int receivedBytes) const;
	};
}

#endif
