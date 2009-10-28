#ifndef _SERIALISE_BASE_H_
#define _SERIALISE_BASE_H_

#include "chipmunk.h"

#define SERIALISED_FLOAT_SIZE 4
#define SERIALISED_DOUBLE_SIZE 8
#define SERIALISED_BOOL_SIZE 1
#define SERIALISED_INT_SIZE 4
#define SERIALISED_SHORT_SIZE 2
#define SERIALISED_VECTOR_SIZE 16

namespace WiredMunk {
	
	/**
	 * Base class for all objects that can be serialised.  Includes static
	 * methods for serialising and deserialising primitive and Chipmunk types.
	 */
	class SerialiseBase {
	public:
		
		/**
		 * Stores a serialised representation of the object.  The buffer must be
		 * large enough to contain the serialised data.  The size of the data
		 * can be found by using the getSerialisedLength() function.
		 * @param buffer Buffer in which to store serialised data.
		 * @return The size of the data in serialised form, in bytes.
		 */
		virtual unsigned int serialise(unsigned char* buffer) = 0;
		
		/**
		 * Deserialises the data.  Should be called by the object's constructor
		 * in order to create a new object that contains the deserialised data.
		 * @param data Data to deserialise.
		 * @return The size of the data deserialised, in bytes.
		 */
		virtual unsigned int deserialise(const unsigned char* data) = 0;
		
		/**
		 * Get the length in bytes of the serialised data.
		 * @return The length in bytes of the serialised data.
		 */
		virtual unsigned int getSerialisedLength() = 0;
		
		/**
		 * Turns a short (2 bytes) into 2 chars.  Supplied output parameter must
		 * point to allocated memory at least 2 bytes long.
		 * @param value Short to serialise.
		 * @param output Char array in which to store serialised short.
		 * @return Number of bytes stored in the output buffer.
		 */
		static unsigned int serialise(unsigned short value, unsigned char* output);
		
		/**
		 * Extracts a short (2 bytes) from the supplied char array.
		 * @param data Data to extract short from.
		 * @return The deserialised short.
		 */
		static unsigned short deserialiseShort(const unsigned char* data);
		
		/**
		 * Turns an int (4 bytes) into 4 chars.  Supplied output parameter must
		 * point to allocated memory at least 4 bytes long.  Integer is stored
		 * in big endian order.
		 * @param value Integer to serialise.
		 * @param output Char array in which to store serialised int.
		 * @return Number of bytes stored in the output buffer.
		 */
		static unsigned int serialise(unsigned int value, unsigned char* output);
		
		/**
		 * Extracts an int (4 bytes) from the supplied char array.
		 * @param data Data to extract int from.
		 * @return The deserialised int.
		 */
		static unsigned int deserialiseInt(const unsigned char* data);
		
		/**
		 * Turns a bool (1 bit) into 1 chars.  Supplied output parameter must
		 * point to allocated memory at least 1 byte long.
		 * @param value Bool to serialise.
		 * @param output Char array in which to store serialised bool.
		 * @return Number of bytes stored in the output buffer.
		 */
		static unsigned int serialise(bool value, unsigned char* output);
		
		/**
		 * Extracts a bool (1 bit) from the supplied char array.
		 * @param data Data to extract bool from.
		 * @return The deserialised bool.
		 */
		static bool deserialiseBool(const unsigned char* data);
		
		/**
		 * Turns a double (8 bytes) into 8 chars.  Supplied output parameter must
		 * point to allocated memory at least 8 bytes long.
		 * @param value Double to serialise.
		 * @param output Char array in which to store serialised double.
		 * @return Number of bytes stored in the output buffer.
		 */
		static unsigned int serialise(double value, unsigned char* output);
		
		/**
		 * Extracts a double (8 bytes) from the supplied char array.
		 * @param data Data to extract double from.
		 * @return The deserialised double.
		 */
		static double deserialiseDouble(const unsigned char* data);
		
		/**
		 * Turns a float (4 bytes) into 4 chars.  Supplied output parameter must
		 * point to allocated memory at least 4 bytes long.
		 * @param value Float to serialise.
		 * @param output Char array in which to store serialised float.
		 * @return Number of bytes stored in the output buffer.
		 */
		static unsigned int serialise(float value, unsigned char* output);
		
		/**
		 * Extracts a float (4 bytes) from the supplied char array.
		 * @param data Data to extract float from.
		 * @return The deserialised float.
		 */
		static float deserialiseFloat(const unsigned char* data);
		
		/**
		 * Turns a cpVect struct (8 bytes) into 8 chars.  Supplied output
		 * parameter must point to allocated memory at least 8 bytes long.
		 * @param vector cpVect to serialise.
		 * @param output Char array in which to store serialised cpVect.
		 * @return Number of bytes stored in the output buffer.
		 */
		static unsigned int serialise(const cpVect& vector, unsigned char* output);
		
		/**
		 * Extracts a cpVect (8 bytes) from the supplied char array.
		 * @param data Data to extract cpVect from.
		 * @return The deserialised cpVect.
		 */
		static cpVect deserialiseVector(const unsigned char* data);
		
	private:
		
		/**
		 * Packs a float or double into IEEE-754 format.
		 * Taken from http://beej.us/guide/bgnet/output/html/multipage/advanced.html#serialization
		 * @param f Value to pack.
		 * @param bits Number of bits in the value.
		 * @param expbits Number of bits in the exponent.
		 */
		static long long pack754(long double f, unsigned bits, unsigned expbits);
		
		/**
		 * Unpacks an IEEE-754 value into a float or double.
		 * Taken from http://beej.us/guide/bgnet/output/html/multipage/advanced.html#serialization
		 * @param i Value to unpack.
		 * @param bits Number of bits in the value.
		 * @param expbits Number of bits in the exponent.
		 */
		static long double unpack754(long long i, unsigned bits, unsigned expbits);
	};
}

#endif
