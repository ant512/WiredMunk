#ifndef _SHAPE_H_
#define _SHAPE_H_

// Modelled on http://www.slembcke.net/forums/viewtopic.php?f=6&t=231&p=1047&hilit=wrapper#p1047

#include "chipmunk.h"
#include "body.h"
#include "boundingbox.h"
#include "serialisebase.h"
#include "networkobject.h"
#include "space.h"

namespace WiredMunk {
	
	class Shape : public NetworkObject {
	public:
		
		/**
		 * Constructor.  Creates a circular shape.
		 * @param body Body attached to the shape.
		 * @param radius Radius of the body,
		 * @param offset
		 */
		Shape(Body* body, cpFloat radius, cpVect offset);
		
		/**
		 * Constructor.  Creates a segment shape.
		 * @param body Body attached to the shape.
		 * @param a
		 * @param b
		 * @param radius
		 */
		Shape(Body* body, cpVect a, cpVect b, cpFloat radius);
		
		/**
		 * Constructor.  Creates a poly shape.
		 * @param body Body attached to the shape.
		 * @param numVerts
		 * @param verts
		 * @param offset
		 */
		Shape(Body* body, int numVerts, cpVect* verts, cpVect offset);
		
		/**
		 * Constructor.  Converts a serialised representation of an object back
		 * into an object. The serialised data does not contain a serialised
		 * body - instead it contains the unique ID of the body, which
		 * references an existing body in the space.
		 * @param bodyVector Vector of bodies from the containing space.
		 * @param staticBodyVector Vector of static bodies from the containing
		 * space.
		 * @param serialisedData The serialised data.
		 */
		Shape(BodyVector* bodyVector, BodyVector* staticBodyVector, const unsigned char* serialisedData);
		
		/**
		 * Destructor.
		 */
		~Shape();
		
		/**
		 * Updates and returns the shape's bounding box.
		 * @return The shape's bounding box.
		 */
		BoundingBox cacheBoundingBox();
		
		/**
		 * Resets the shape ID counter.  Should be called every time a space is
		 * populated with new shapes.
		 */
		void resetShapeIdCounter();
		
		/**
		 * Get a pointer to the Chipmunk cpShape struct.
		 * @return A pointer to the Chipmunk cpShape struct.
		 */
		inline cpShape* getShape() { return _shape; };
		
		/**
		 * Get the shape's ID.
		 * @return The shape's ID.
		 */
		inline unsigned int getId() const { return _shape->id; };
		
		/**
		 * Get the shape's bounding box.
		 * @return The shape's bounding box.
		 */
		BoundingBox getBoundingBox();
		
		/**
		 * Get the shape's collision type.
		 * @return The shape's collision type.
		 */
		inline unsigned int getCollisionType() const { return _shape->collision_type; };
		
		/**
		 * Get the shape's collision group.
		 * @return The shape's collision group.
		 */
		inline unsigned int getCollisionGroup() const { return _shape->group; };
		
		/**
		 * Get the shape's collision layers.
		 * @return The shape's collision layers.
		 */
		inline unsigned int getCollisionLayers() const { return _shape->layers; };
		
		/**
		 * Get the shape's user data.
		 * @return The shape's user data.
		 */
		inline void* getUserData() { return _shape->data; };
		
		/**
		 * Get the shape's body.
		 * @return The shape's body.
		 */
		inline Body* getBody() { return _body; };
		
		/**
		 * Get the shape's elasticity.
		 * @return The shape's elasticity.
		 */
		inline cpFloat getElasticity() const { return _shape->e; };
		
		/**
		 * Get the shape's friction.
		 * @return The shape's friction.
		 */
		inline cpFloat getFriction() const { return _shape->u; };
		
		/**
		 * Get the shape's surface velocity.
		 * @return The shape's surface velocity.
		 */
		inline cpVect getSurfaceVelocity() const { return _shape->surface_v; };
		
		/**
		 * Set the shape's collision type.
		 * @param type The collision type.
		 */
		void setCollisionType(unsigned int type);
		
		/**
		 * Set the shape's collision group.
		 * @param group The shape's collision group.
		 */
		void setCollisionGroup(unsigned int group);
		
		/**
		 * Set the shape's collision layers.
		 * @param layers The shape's collision layers.
		 */
		void setCollisionLayers(unsigned int layers);
		
		/**
		 * Set the shape's user data.
		 * @param data The shape's user data.
		 */
		void setUserData(void* data);
		
		/**
		 * Set the shape's elasticity.
		 * @param elasticity The shape's elasticity.
		 */
		void setElasticity(cpFloat elasticity);
		
		/**
		 * Set the shape's friction.
		 * @param friction The shape's friction.
		 */
		void setFriction(cpFloat friction);
		
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
		 * @param bodyVector Vector of bodies from the containing space.
		 * @param staticBodyVector Vector of static bodies from the containing
		 * space.
		 * @param data Data to deserialise.
		 * @return The size of the data deserialised, in bytes.
		 */
		virtual unsigned int deserialise(BodyVector* bodyVector, BodyVector* staticBodyVector, const unsigned char* data);
		
		/**
		 * Get the length in bytes of the serialised data.
		 * @return The length in bytes of the serialised data.
		 */
		unsigned int getSerialisedLength();
		
		/**
		 * Transmit the object in serialised form across the network.
		 * @param address Address to send the object to.
		 */
		virtual void sendObject(const struct sockaddr_in* address);
		
	protected:
		cpShape* _shape;			/**< The Chipmunk shape */
		Body* _body;				/**< The shape's body */
	};
}

#endif
