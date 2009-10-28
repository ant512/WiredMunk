#ifndef _CLIENT_MANAGER_H_
#define _CLIENT_MANAGER_H_

#include "socketeventhandler.h"
#include "clientlist.h"
#include "socket.h"
#include "message.h"
#include "space.h"

namespace WiredMunk {

	/**
	 * Class that manages clients and client communications.
	 */
	class ClientManager : public SocketEventHandler {
	public:
		
		/**
		 * Constructor.
		 * @param socket Pointer to a socket used for communicating with
		 * clients.
		 * @param clientCount Number of clients required to connect.
		 */
		ClientManager(Socket* socket, int clientCount);
		
		/**
		 * Handles incoming messages from the socket.
		 * @param msg Message data.
		 */
		void handleMessageReceived(const Message& msg);
		
		/**
		 * Sends the simulated space to all clients.
		 * @param space Space to transmit.
		 */
		void sendSpace(Space* space);
		
	private:
		ClientList _clients;			/**< List of clients */
		Socket* _socket;				/**< Socket for client communication */
		int _clientCount;				/**< Number of expected clients */
		int _readyClientCount;			/**< Number of clients ready to start */
		
		/**
		 * Add a client to the list of clients.
		 * @param address The client's address.
		 */
		void addClient(const struct sockaddr_in* address);
		
		/**
		 * Receives handshake requests from clients and responds with a
		 * message containing a unique client id.  If the simulation is full,
		 * the client is sent a rejection message instead.
		 * @param msg Message data.
		 */
		void handleHandshakeReceived(const Message& msg);
		
		/**
		 * Receives ready notifications from clients.  These are sent when
		 * clients are ready to start the simulations.  When all clients are
		 * ready, this function sends a ready message back to all clients
		 * informing them that the simulation can start.
		 * @param msg Message data.
		 */
		void handleReadyReceived(const Message& msg);
		
		/**
		 * Receives requests for unique object ids from the clients.  Responds
		 * with a unique id.
		 * @param msg Message data.
		 */
		void handleObjectIdRequestReceived(const Message& msg);
	};
}

#endif
