#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include "chipmunk.h"
#include "serialisebase.h"

namespace WiredMunk {

	/**
	 * Wrapper around the Chipmunk cpBB struct.  Represents a bounding box.
	 */
	class BoundingBox : public SerialiseBase {
	public:
		
		/**
		 * Constructor.
		 */
		BoundingBox();
		
		/**
		 * Constructor.
		 * @param box A cpBB box.
		 */
		BoundingBox(cpBB box);
		
		/**
		 * Constructor.
		 * @param left Left edge of the box.
		 * @param bottom Bottom edge of the box.
		 * @param right Right edge of the box.
		 * @param top Top edge of the box.
		 */
		BoundingBox(const cpFloat left, const cpFloat bottom, const cpFloat right, const cpFloat top);
		
		/**
		 * Constructor.  Converts a serialised representation of an object back
		 * into an object.
		 */
		BoundingBox(const unsigned char* serialisedData);
		
		/**
		 * Destructor
		 */
		~BoundingBox();
		
		/**
		 * Check if this box intersects the supplied box.
		 * @param box Box to check intersects with.
		 * @return True if the box intersects this.
		 */
		bool intersects(const BoundingBox& box) const;
		
		/**
		 * Check if this box contains the supplied box.
		 * @param box Box this box may contain.
		 * @return True if the box is completely surrounded by this.
		 */
		bool containsBoundingBox(const BoundingBox& box) const;
		
		/**
		 * Check if this box contains the supplied vector.
		 * @param vector Vector to check.
		 * @return True if the vector is completely surrounded by this box.
		 */
		bool containsVect(const cpVect& vector) const;
		
		/**
		 * Limits the vector to fit inside the confines of the box.
		 * @param vector Vector to clamp.
		 * @return A clamped version of the vector.
		 */
		cpVect clampVect(const cpVect& vector) const;
		
		/**
		 * Wraps the vector around the box's dimensions if it exceeds the box.
		 * @param vector Vector to wrap.
		 * @return A wrapped version of the vector.
		 */
		cpVect wrapVect(const cpVect& vector) const;
		
		/**
		 * Get a pointer to the cpBB box.
		 * @return A pointer to the cpBB box.
		 */
		cpBB getBoundingBox() const;
		
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
		
	private:
		cpBB _boundingBox;							/**< The bounding box */
	};
}

#endif
