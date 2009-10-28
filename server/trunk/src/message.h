#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define MESSAGE_HEADER "WDMK"
#define MESSAGE_HEADER_LENGTH 9

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/**
 * Message format:
 * 4 byte header: WDMK (identify message as a wired munk header)
 * 1 byte message type
 * 2 byte message length
 * 2 byte id number
 * n bytes data
 */

namespace WiredMunk {
	
	/**
	 * Messages that are to be sent across the network should be sent as an
	 * instance of this class.  The message class formats the data into a
	 * predefined structure that can be easily parsed and extracted at the
	 * recipient.  Each message has a type so that its purpose is clearly
	 * identified.
	 */
	class Message {
	public:
		
		/**
		 * Enum listing all types of message that can be sent/received.
		 */
		enum MessageType {
			MESSAGE_NONE = 1,				/**< No message type; included for completeness */
			MESSAGE_HANDSHAKE = 2,			/**< Handshake between client and server sent at client startup */
			MESSAGE_REJECT = 3,				/**< Server rejects client's handshake because the session is full */
			MESSAGE_STARTUP = 4,			/**< Sent to clients to tell them to run their startup() method */
			MESSAGE_READY = 5,				/**< Sent to server to indicate client readiness and to clients to start session */
			MESSAGE_PING = 6,				/**< Not implemented */
			MESSAGE_ACKNOWLEDGE = 7,		/**< Not implemented */
			MESSAGE_OBJECT_ID = 8,			/**< Sent if client requesting a unique ID for an object */
			MESSAGE_BODY = 9,				/**< Message contains body data */
			MESSAGE_SHAPE = 10,				/**< Message contains shape data */
			MESSAGE_SPACE = 11				/**< Message contains space data */
		};
		
		/**
		 * Constructor.
		 * @param type The type of the message.
		 * @param dataLength The length of the message data.
		 * @param data The message data.
		 * @param address The address that the message came from or is being
		 * sent to.
		 * @param fromAddress
		 */
		Message(MessageType type, unsigned short msgId, unsigned short dataLength, const unsigned char* data, const struct sockaddr_in* address);
		
		/**
		 * Constructor.  Automatically splits data string into individual
		 * message components.
		 * @param data Data string containing message.
		 * @param address Address to send to or address that message came from.
		 */
		Message(const unsigned char* data, const struct sockaddr_in* address);
		
		/**
		 * Copy constructor.
		 * @param copy Message to copy.
		 */
		Message(Message const& copy);
		
		/**
		 * Destructor.
		 */
		~Message();
		
		/**
		 * Get the message type.
		 * @return The message type.
		 */
		inline const MessageType getType() const { return _type; };
		
		/**
		 * Get the message data.
		 * @return The message data.
		 */
		inline const unsigned char* getData() const { return _data; };
		
		/**
		 * Get the ID of the message.
		 * @return The ID of the message.
		 */
		inline unsigned short getId() const { return _id; };
		
		/**
		 * Get the length of the message data.
		 * @return The length of the message data.
		 */
		inline unsigned short getDataLength() const { return _dataLength; };

		/**
		 * Get the to/from address, depending on if the message is being sent or
		 * has been received.
		 * @return The address of the message.
		 */
		inline const struct sockaddr_in* getAddress() const { return &_address; };
		
		/**
		 * Get the entire message, with prefixed header, ready for transmission.
		 * @param buffer Pointer to a buffer to fill with message data.
		 * Buffer must be large enough to contain the entire formatted message;
		 * use getFormattedMessageLength() to obtain this value.
		 * @return Length of the message data.
		 */
		unsigned int getFormattedMessage(unsigned char* buffer) const;
		
		/**
		 * Get the length of the formatted message.
		 */
		unsigned int getFormattedMessageLength() const;
		
		/**
		 * Sets the message data.
		 * @param dataLength The message data.
		 * @param length The length of the message data.
		 */
		void setData(const unsigned char* data, unsigned short dataLength);
		
		/**
		 * Sets the message address.
		 * @param address The message address.
		 */
		void setAddress(const struct sockaddr_in* address);

	private:
		unsigned short _dataLength;				/**< Length of the data component */
		MessageType _type;						/**< Type of message */
		unsigned char* _data;					/**< Message data */
		unsigned short _id;						/**< Message ID */
		struct sockaddr_in _address;			/**< The address the message was sent from/is being sent to */
	};
}

#endif
