#ifndef _ID_SERVER_H_
#define _ID_SERVER_H_

#include "debug.h"

namespace WiredMunk {

	/**
	 * Generates unique ID numbers for clients and network objects.
	 */
	class IDServer {
	public:
		
		/**
		 * Get a unique client ID.
		 * @return A unique client ID.
		 */
		inline static unsigned int getNextClientId() {
			Debug::printf("Generated client ID: %d\n", _clientId);
					
			return _clientId++;
		};
		
		/**
		 * Get a unique network object ID.
		 * @return A unique network object ID.
		 */
		inline static unsigned int getNextNetworkObjectId() {
			Debug::printf("Generated network object ID: %d\n", _networkObjectId);
			
			return _networkObjectId++;
		};
		
	private:
		static unsigned int _clientId;			/**< The next client ID */
		static unsigned int _networkObjectId;	/**< The next network object ID */
	};
}

#endif
