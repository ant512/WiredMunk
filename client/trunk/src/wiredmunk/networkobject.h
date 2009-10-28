#ifndef _NETWORK_OBJECT_H_
#define _NETWORK_OBJECT_H_

#include "socketeventhandler.h"
#include "serialisebase.h"

namespace WiredMunk {

	/**
	 * Class containing data pertinent to the network.  Represents an object
	 * that is duplicated across the network.
	 */
	class NetworkObject : public SocketEventHandler, public SerialiseBase {
	public:
		
		/**
		 * Constructor.
		 */
		NetworkObject();
		
		/**
		 * Constructor.  Converts a serialised representation of an object back
		 * into an object.
		 * @param serialisedData Data to deserialise.
		 */
		NetworkObject(const unsigned char* serialisedData);

		/**
		 * Gets the object's id.  The id is unique across the network.
		 * @return The object's id.
		 */
		inline unsigned int getObjectId() const { return _objectId; };
		
		/**
		 * Process response received events.  Checks for incoming object ID.
		 * @param msg Message to be processed.
		 */
		virtual void handleResponseReceived(const Message& msg);
		
		/**
		 * Stores a serialised representation of the object.  The buffer must be
		 * large enough to contain the serialised data.  The size of the data
		 * can be found by using the getSerialisedLength() function.
		 * @param buffer Buffer in which to store serialised data.
		 * @return The size of the data in serialised form, in bytes.
		 */
		virtual unsigned int serialise(unsigned char* buffer);
		
		/**
		 * Deserialises the data.  Should be called by the object's constructor
		 * in order to create a new object that contains the deserialised data.
		 * @param data Data to deserialise.
		 * @return The size of the data deserialised, in bytes.
		 */
		virtual unsigned int deserialise(const unsigned char* data);
		
		/**
		 * Get the length in bytes of the serialised data.
		 * @return The length in bytes of the serialised data.
		 */
		virtual unsigned int getSerialisedLength();
		
		/**
		 * Transmit the object in serialised form across the network.  Must be
		 * overridden, as each sub-object needs to define its own message type
		 * in order for the data to be deserialised correctly.
		 */
		virtual void sendObject() = 0;
		
		/**
		 * Check if the object has been manually altered and is in need of
		 * transmission to the server.
		 */
		bool isAltered() const { return _isAltered; };
		
		/**
		 * Set the object's altered state.
		 * @param altered The object's altered state.
		 */
		void setAltered(bool altered) { _isAltered = altered; };
		
	protected:
		
		/**
		 * Set the object ID.
		 * @param objectId The object ID.
		 */
		void setObjectId(unsigned int objectId);
		
	private:		
		unsigned int _objectId;				/**< The object's id, unique across the network */
		static unsigned int _nextId;		/**< The next ID to be generated */
		bool _isAltered;					/**< Has the object been manually altered? */
		
		/**
		 * Requests a unique ID for the object from the server.
		 */
		void requestObjectId();
		
		/**
		 * Get the next unique ID.
		 * @return The next unique ID.
		 */
		inline static unsigned int getNextId() { return _nextId++; };
	};
}

#endif
