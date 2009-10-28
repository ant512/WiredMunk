#ifndef _SOCKET_EVENT_HANDLER_H_
#define _SOCKET_EVENT_HANDLER_H_

namespace WiredMunk {
	
	class Message;
	
	/**
	 * Base class for classes that need to listen for events raised by the
	 * Socket class.
	 */
	class SocketEventHandler {
	public:
		
		/**
		 * Process message received events.  Should be overridden.
		 * @param msg Message to be processed.
		 */
		virtual void handleMessageReceived(const Message& msg) { };
		
		/**
		 * Process reply received events.  Should be overridden.
		 * @param msg Message to be processed.
		 */
		virtual void handleResponseReceived(const Message& msg) { };
	};
}

#endif
