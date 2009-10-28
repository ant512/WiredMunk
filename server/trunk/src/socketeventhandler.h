#ifndef _SOCKET_EVENT_HANDLER_H_
#define _SOCKET_EVENT_HANDLER_H_

#include "socketeventargs.h"
#include "message.h"

namespace WiredMunk {

	/**
	 * Base class for classes that need to listen for events raised by the
	 * Socket class.
	 */
	class SocketEventHandler {
	public:
		
		/**
		 * Process message received events.  Should be overridden.
		 * @param e Message to be processed.
		 */
		virtual void handleMessageReceived(const Message& msg) { };
	};
}

#endif
