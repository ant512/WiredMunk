#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "socketeventhandler.h"
#include "message.h"

#define MESSAGE_BUFFER_LENGTH 16384

namespace WiredMunk {

	/**
	 * Represents a socket that can be opened to connect to a server.  Socket is
	 * bidirectional and messages can be both sent and received.  Uses UDP
	 * datagrams for communication.  All calls are non-blocking.
	 */
	class Socket {
	public:
		
		/**
		 * Constructor.
		 */
		inline Socket() { };
		
		/**
		 * Open a connection.
		 * @param hostName IP address of the computer to connect to.
		 * @param portNum Port number of the computer to connect to.
		 * @return True if the socket connected successfully.
		 */
		bool open(const char* hostName, const int portNum);
		
		/**
		 * Close the socket.
		 */
		void shut();
		
		/**
		 * Destructor.
		 */
		~Socket();
		
		/**
		 * Check for incoming data from socket.  Raises a message received event
		 * if any messages are found.
		 * @return The number of bytes received.
		 */
		int poll();
		
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
		void sendMessage(const Message* msg);
		
	private:
		int _socket;										/**< Socket file descriptor */
		struct sockaddr_in _server;							/**< Address of the server */
		std::vector<SocketEventHandler*> _eventHandlers;	/**< List of event handlers */
		std::vector<Message*> _responsePendingMessages;		/**< List of messages awaiting a response */
		
		/**
		 * Write data to the socket.
		 * @param data Data to write.
		 * @param length Length of the data.
		 */
		bool write(const unsigned char* data, unsigned int length) const;
		
		/**
		 * Raise a message received event to all event handlers.
		 * @param data Data received in message.
		 * @param receivedBytes Number of bytes received.
		 */
		void raiseMessageReceivedEvent(unsigned char* data, int receivedBytes) const;
		
		/**
		 * Checks incoming messages to see if they are replies to existing
		 * messages; if so, the replies are distributed appropriately.
		 * @param data Data received in message.
		 * @param receivedBytes Number of bytes received.
		 * @return True if the message was handled as a reply; false if not.
		 */
		bool handleReply(unsigned char* data, int receivedBytes);
	};
}

#endif
