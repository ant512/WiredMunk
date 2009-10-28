#ifndef _WIRED_MUNK_APP_H_
#define _WIRED_MUNK_APP_H_

#include <sys/time.h>

#include "socket.h"
#include "socketeventhandler.h"
#include "message.h"
#include "space.h"

#define REFRESH_RATE 85.0

namespace WiredMunk {
	
	class PositionSampler;
	
	/**
	 * WiredMunk application class.  Users should subclass this and use the
	 * subclass as the basis of their programs.
	 */
	class WiredMunkApp : public SocketEventHandler {
	public:
		
		/**
		 * Enum of all states client can be in.
		 */
		enum ClientState {
			CLIENT_STATE_NEW,					/**< Client is newly created */
			CLIENT_STATE_WAITING_HANDSHAKE,		/**< Client is awaiting handshake with server */
			CLIENT_STATE_WAITING_STARTUP,		/**< Client is awaiting permission to startup from server */
			CLIENT_STATE_STARTING,				/**< Client is running startup code */
			CLIENT_STATE_WAITING_READY,			/**< Client is awaiting ready message from server */
			CLIENT_STATE_RUNNING				/**< Client is running main code */
		};
		
		/**
		 * Constructor.
		 * @param serverIP IP address of the server.
		 * @param portNum Port number to connect to server on.
		 */
		WiredMunkApp(const char* serverIP, int portNum);
		
		/**
		 * Destructor.
		 */
		~WiredMunkApp();
		
		/**
		 * Runs a single iteration of the program.  Should be called in a loop.
		 */
		void run();
		
		/**
		 * Get a pointer to the socket used for server communication.
		 * @return A pointer to the socket.
		 */
		Socket* getSocket();
		
		/**
		 * Process reply received events.
		 * @param msg Message to be processed.
		 */
		virtual void handleResponseReceived(const Message& msg);
		
		/**
		 * Process message received events.
		 * @param msg Message to be processed.
		 */
		virtual void handleMessageReceived(const Message& msg);
		
		/**
		 * Get a pointer to the app singleton.
		 * @return A pointer to the app singleton.
		 */
		inline static WiredMunkApp* getApp() { return _singleton; };
		
		/**
		 * Get the client's state.
		 * @return The client's state.
		 */
		inline ClientState getClientState() { return _clientState; };
		
		/**
		 * Get a pointer to the space.
		 * @return A pointer to the space.
		 */
		inline Space* getSpace() { return _space; };
		
		/**
		 * Get the client ID.
		 * @return The client ID.
		 */
		inline int getClientId() { return _clientId; };
		
	protected:
		Space* _space;						/**< Simulation space */
		
		/**
		 * Startup code.
		 */
		virtual void startup() { };
		
		/**
		 * Shutdown code.
		 */
		virtual void shutdown() { };
		
		/**
		 * Should contain any code that the user needs to run every loop whilst
		 * simulation is executing.
		 */
		virtual void runUser() { };
		
	private:
		Socket _socket;						/**< Socket connected to the server */
		ClientState _clientState;			/**< Current state of the client */
		int _clientId;						/**< Client's unique ID number */
		struct timeval _lastRunTime;		/**< Time that the simulation was last stepped */
		
		static WiredMunkApp* _singleton;	/**< Singleton instance of the app */
		
		PositionSampler* _sampler;
		
		/**
		 * Handshake with the server.  Requests an ID for this client.
		 */
		void requestHandshake();
		
		/**
		 * Informs server that client is ready to begin.
		 */
		void sendReady();
		
		/**
		 * Serialises the space and sends it to the server.
		 */
		void sendSpace();
		
		/**
		 * Send any manually-altered objects to the server.
		 */
		void sendAlteredObjects();
		
		/**
		 * Steps the space.
		 */
		void stepSpace();
		
		/**
		 * Resets the altered state of all objects to unaltered.  Called once
		 * the initial startup routine has run.
		 */
		void resetAlteredState();
	};
}

#endif
