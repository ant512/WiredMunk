#include <iostream>
#include "socket.h"
#include "clientmanager.h"
#include "simulation.h"

namespace WiredMunk {
	
	/**
	 * Server class.  Runs the simulation and maintains communication with
	 * clients via the ClientManager class.  Contains the socket through which
	 * all communication takes place.
	 *
	 * Only one instance should be created.
	 */
	class Server {
	public:
		
		/**
		 * Constructor.
		 * @param clientCount Number of clients required for a session.
		 * @param portNum Port to open server on.
		 */
		Server(int clientCount, int portNum);
		
		/**
		 * Destructor.
		 */
		~Server();
		
		/**
		 * Main loop.
		 */
		virtual void run();
		
		/**
		 * Get a pointer to the singleton.
		 * @return A pointer to the server singleton.
		 */
		static Server* getServer() { return _singleton; };
		
		/**
		 * Get a pointer to the socket.
		 * @return A pointer to the socket.
		 */
		Socket* getSocket() { return _socket; };
		
		/**
		 * Get a pointer to the client manager.
		 * @return The client manager.
		 */
		ClientManager* getClientManager() { return _clientManager; };
		
	private:
		Socket* _socket;				/**< Socket used for comms */
		ClientManager* _clientManager;	/**< Client management */
		Simulation* _simulation;		/**< Server-side physics simulation */
		
		static Server* _singleton;		/**< Single server instance */
	};
}
