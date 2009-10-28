#ifndef _JOINT_H_
#define _JOINT_H_

#include "chipmunk.h"
#include "body.h"
#include "networkobject.h"

namespace WiredMunk {
	
	/**
	 * Wrapper around the cpJoint struct and functions.  Represents a joint
	 * between two bodies.
	 *
	 * Basic wrapping modelled on http://www.slembcke.net/forums/viewtopic.php?f=6&t=231&p=1047&hilit=wrapper#p1047
	 */
	class Joint : public NetworkObject  {
	public:
		
		/**
		 * Constructor.  Creates a pin joint.
		 * @param body1 First body to join.
		 * @param body2 Second body to join.
		 * @param anchor1 Offset from centre of first body to place joint.
		 * @param anchor2 Offset from centre of second body to place joint.
		 */
		Joint(Body* body1, Body* body2, cpVect anchor1, cpVect anchor2);
		
		/**
		 * Constructor.  Creates a slide joint.
		 * @param body1 First body to join.
		 * @param body2 Second body to join.
		 * @param anchor1 Offset from centre of first body to place joint.
		 * @param anchor2 Offset from centre of second body to place joint.
		 * @param min Minimum distance between the two anchors.
		 * @param max Maximum distance between the two anchors.
		 */
		Joint(Body* body1, Body* body2, cpVect anchor1, cpVect anchor2, cpFloat min, cpFloat max);
		
		/**
		 * Constructor.  Creates a pivot joint.
		 * @param body1 First body to join.
		 * @param body2 Second body to join.
		 * @param pivot Point in world co-ordinates of the pivot.
		 */
		Joint(Body* body1, Body* body2, cpVect pivot);
		
		/**
		 * Constructor.  Creates a groove joint.
		 * @param body1 First body to join.
		 * @param body2 Second body to join.
		 * @param grooveA Location on body1 of the start of the groove.
		 * @param grooveB Location on body1 of the end of the groove.
		 * @param anchor Location on body2 of the pivot position.
		 */
		Joint(Body* body1, Body* body2, cpVect grooveA, cpVect grooveB, cpVect anchor);
		
		/**
		 * Constructor.  Converts a serialised representation of an object back
		 * into an object.
		 */
		Joint(const unsigned char* serialisedData);
		
		/**
		 * Destructor.
		 */
		~Joint();
		
		/**
		 * Get a pointer to the Chipmunk joint.
		 * @return The Chipmunk joint.
		 */
		cpJoint* getJoint();
		
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
		cpJoint* _joint;								/**< The joint */
	};
}

#endif
