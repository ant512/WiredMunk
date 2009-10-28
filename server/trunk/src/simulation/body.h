#ifndef _BODY_H_
#define _BODY_H_

/**
 * Modelled on http://www.slembcke.net/forums/viewtopic.php?f=6&t=231&p=1047&hilit=wrapper#p1047
 * Comments based on http://code.google.com/p/chipmunk-physics/wiki/cpBody
 */

#include "chipmunk.h"
#include "networkobject.h"

namespace WiredMunk {
	
	/**
	 * Wrapper around the cpBody struct and set of functions.  Represents a
	 * rigid body.
	 */
	class Body : public NetworkObject {
	public:
		
		/**
		 * Constructor.
		 */
		Body();
		
		/**
		 * Constructor.
		 * @param mass Mass of the body.
		 * @param moment Moment of intertia, or resistance to changes in body's
		 * rotation rate.
		 */
		Body(cpFloat mass, cpFloat moment);
		
		/**
		 * Constructor.
		 * @param body Pointer to an existing cpBody.
		 */
		Body(cpBody* body);
		
		/**
		 * Constructor.  Converts a serialised representation of an object back
		 * into an object.
		 */
		Body(const unsigned char* serialisedData);
		
		/**
		 * Destructor.
		 */
		~Body();
		
		/**
		 * Set the body's mass.
		 * @param mass The new mass.
		 */
		void setMass(cpFloat mass);
		
		/**
		 * Set the body's moment.
		 * @param moment The body's moment.
		 */
		void setMoment(cpFloat moment);
		
		/**
		 * Set the body's angle.
		 * @param angle The body's angle.
		 */
		void setAngle(cpFloat angle);
		
		/**
		 * Set the body's position.
		 * @param position The body's position.
		 */
		void setPosition(cpVect position);
		
		/**
		 * Set the body's velocity.
		 * @param velocity The body's velocity.
		 */
		void setVelocity(cpVect velocity);
		
		/**
		 * Set the body's angular velocity.
		 * @param velocity The body's angular velocity.
		 */
		void setAngularVelocity(cpFloat velocity);
		
		/**
		 * Set the body's torque, or the angular force acting on the body.
		 * @param torque The torque acting on the body.
		 */
		void setTorque(cpFloat torque);
		
		/**
		 * Modify the velocity of the body so that it will move to the specified
		 * absolute coordinates in the next timestep. Intended for objects that
		 * are moved manually with a custom velocity integration function.
		 * @param position Position to move to.
		 * @param dt Time required for movement.
		 */
		void slew(cpVect position, cpFloat dt);
		
		/**
		 * Default rigid body velocity integration function.
		 * @param gravity 
		 * @param damping
		 * @param dt
		 */
		void updateVelocity(cpVect gravity, cpFloat damping, cpFloat dt);
		
		/**
		 * Default rigid body position integration function.
		 * @param dt
		 */
		void updatePosition(cpFloat dt);
		
		/**
		 * Get the body's mass
		 * @return The body's mass.
		 */
		inline cpFloat getMass() const { return _body->m; };
		
		/**
		 * Get the body's moment.
		 * @return The body's moment.
		 */
		inline cpFloat getMoment() const { return _body->i; };
		
		/**
		 * Get the body's force.
		 * @return The body's force.
		 */
		inline cpVect getForce() const { return _body->f; };
		
		/**
		 * Get the body's angle.
		 * @return The body's angle.
		 */
		inline cpFloat getAngle() const { return _body->a; };
		
		/**
		 * Get the body's vector angle.
		 * @return The body's vector angle.
		 */
		inline cpVect getVecAngle() const { return _body->rot; };
		
		/**
		 * Get the body's torque.
		 * @return The body's torque.
		 */
		inline cpFloat getTorque() const { return _body->t; };
		
		/**
		 * Get the body's position.
		 * @return The body's position.
		 */
		inline cpVect getPosition() const { return _body->p; };
		
		/**
		 * Get the body's velocity.
		 * @return The body's velocity.
		 */
		inline cpVect getVelocity() const { return _body->v; };
		
		/**
		 * Get the body's angular velocity.
		 * @return The body's angular velocity.
		 */
		inline cpFloat getAngularVelocity() const { return _body->w; };
		
		/**
		 * Convert the vector expressed in local co-ordinates to a vector
		 * expressed in world co-ordinates.
		 * @param vector Vector to convert.
		 * @return A converted vector.
		 */
		cpVect localToWorld(cpVect vector);
		
		/**
		 * Convert the vector expressed in world co-ordinates to a vector
		 * expressed in local co-ordinates.
		 * @param vector Vector to convert.
		 * @return A converted vector.
		 */
		cpVect worldToLocal(cpVect vector);
		
		/**
		 * Reset the forces acting on the body.
		 */
		void resetForces();
		
		/**
		 * Apply an impulse in world co-ordinates to the body.
		 * @param force Force to apply.
		 * @param offset Offset (in body-relative co-ordinates) from the centre
		 * of the body.
		 */
		void applyImpulse(cpVect force, cpVect offset);
		
		/**
		 * Apply a force in world co-ordinates to the body.
		 * @param force Force to apply.
		 * @param offset Offset (in body-relative co-ordinates) from the centre
		 * of the body.
		 */
		void applyForce(cpVect force, cpVect offset);
		
		/**
		 * Apply torque to a body.
		 * @param torque Torque to apply.
		 */
		void applyTorque(cpFloat torque);
		
		/**
		 * Get a pointer to the Chipmunk body.
		 * @return A pointer to the Chipmunk body.
		 */
		inline cpBody* getBody() const { return _body; };
		
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
		 * Transmit the object in serialised form across the network.
		 * @param address Address to send the object to.
		 */
		virtual void sendObject(const struct sockaddr_in* address);
		
	protected:
		cpBody* _body;				/**< Chipmunk body */
	};
}

#endif
