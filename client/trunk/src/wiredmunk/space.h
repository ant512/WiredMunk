#ifndef _SPACE_H_
#define _SPACE_H_

#include <vector>
#include "chipmunk.h"
#include "networkobject.h"

namespace WiredMunk {

	class Body;
	class Joint;
	class Shape;
	
	typedef std::vector<Body*> BodyVector;
	typedef std::vector<Shape*> ShapeVector;
	typedef std::vector<Joint*> JointVector;
	
	/**
	 * Wrapper around the cpSpace struct and functions.  Represents a virtual
	 * environment.
	 *
	 * Basic wrapping modelled on http://www.slembcke.net/forums/viewtopic.php?f=6&t=231&p=1047&hilit=wrapper#p1047
	 */
	class Space : public NetworkObject {
	public:
		
		/**
		 * Constructor.
		 */
		Space();
		
		/**
		 * Constructor.
		 * @param iterations Number of iterations for iterative solver.
		 */
		Space(int iterations);
		
		/**
		 * Constructor.  Converts a serialised representation of an object back
		 * into an object.
		 */
		Space(const unsigned char* serialisedData);
		
		/**
		 * Destructor.
		 */
		~Space();
		
		/**
		 * Add a shape to the space.
		 * @param shape Shape to add.
		 * @return True if the shape was added; false if not.
		 */
		bool addShape(Shape* shape);
		
		/**
		 * Add a static shape to the space.
		 * @param shape Shape to add.
		 * @return True if the shape was added; false if not.
		 */
		bool addStaticShape(Shape* shape);
		
		/**
		 * Add a body to the space.
		 * @param body Body to add.
		 * @return True if the body was added; false if not.
		 */
		bool addBody(Body* body);
		
		/**
		 * Add a static to the space.
		 * @param body Body to add.
		 * @return True if the body was added; false if not.
		 */
		bool addStaticBody(Body* body);
		
		/**
		 * Add a joint to the space.
		 * @param joint Joint to add.
		 * @return True if the joint was added; false if not.
		 */
		void addJoint(Joint* joint);
		
		/**
		 * Remove a shape from the space.
		 * @param shape Shape to remove.
		 */
		void removeShape(Shape* shape);
		
		/**
		 * Remove a static shape from the space.
		 * @param shape Shape to remove.
		 */
		void removeStaticShape(Shape* shape);
		
		/**
		 * Remove a body from the space.
		 * @param body Body to remove.
		 */
		void removeBody(Body* body);
		
		/**
		 * Remove a joint from the space.
		 * @param joint Joint to remove.
		 */
		void removeJoint(Joint* joint);
		
		/**
		 * Get the space's number of iterations.
		 * @return The space's number of iterations.
		 */
		inline int getIterations() const { return _space->iterations; };
		
		/**
		 * Get the space's gravity.
		 * @return The space's gravity.
		 */
		inline cpVect getGravity() const { return _space->gravity; };
		
		/**
		 * Get the space's damping.
		 * @return The space's damping.
		 */
		inline cpFloat getDamping() const { return _space->damping; };
		
		/**
		 * Get the space's timestamp.
		 * @return The space's timestamp.
		 */
		inline int getTimeStamp() const { return _space->stamp; };
		
		/**
		 * Get the list of shapes.
		 * @return The list of shapes.
		 */
		inline ShapeVector* getShapes() { return &_shapeList; };
		
		/**
		 * Get the list of static shapes.
		 * @return The list of static shapes.
		 */
		inline ShapeVector* getStaticShapes() { return &_staticShapeList; };
		
		/**
		 * Get the list of bodies.
		 * @return The list of bodies.
		 */
		inline BodyVector* getBodies() { return &_bodyList; };
		
		/**
		 * Get the list of static bodies.
		 * @return The list of static bodies.
		 */
		inline BodyVector* getStaticBodies() { return &_staticBodyList; };
		
		/**
		 * Get the list of joints.
		 * @return The list of joints.
		 */
		inline JointVector* getJoints() { return &_jointList; };
		
		/**
		 * Get the Chipmunk space.
		 * @return The Chipmunk space.
		 */
		inline cpSpace* getSpace() const { return _space; };
		
		/**
		 * Set the number of iterations for the iterator solver.
		 * @param iterations The number of iterations.
		 */
		void setIterations(int iterations);
		
		/**
		 * Set the space's gravity.
		 * @param gravity The gravity.
		 */
		void setGravity(cpVect gravity);
		
		/**
		 * Set the space's damping.
		 * @param damping The damping.
		 */
		void setDamping(cpFloat damping);
		
		/**
		 * The spatial hashes used by Chipmunk's collision detection are fairly
		 * size sensitive. dim is the size of the hash cells. Setting dim to
		 * half the average collision shape size is likely to give the best
		 * performance. Setting 'dim' too small will cause the shape to be
		 * inserted into many cells, setting it too low will cause too many
		 * objects into the same slot.
		 *
		 * count is the suggested minimum number of cells in the hash table. If
		 * there are too few cells, the spatial hash will return many false
		 * positives. Too many cells will be hard on the cache and waste memory.
		 * Setting count to ~10x the number of objects in the space is probably
		 * a good starting point. Tune from there if necessary.
		 *
		 * By default, dim is 100.0, and count is 1000.
		 *
		 * http://code.google.com/p/chipmunk-physics/wiki/cpSpace
		 * @param dim Size of the hash cells.
		 * @param count Suggested minimum number of cells in the hash table.
		 */
		void resizeStaticHash(cpFloat dim, int count);
		
		/**
		 * The spatial hashes used by Chipmunk's collision detection are fairly
		 * size sensitive. dim is the size of the hash cells. Setting dim to
		 * half the average collision shape size is likely to give the best
		 * performance. Setting 'dim' too small will cause the shape to be
		 * inserted into many cells, setting it too low will cause too many
		 * objects into the same slot.
		 *
		 * count is the suggested minimum number of cells in the hash table. If
		 * there are too few cells, the spatial hash will return many false
		 * positives. Too many cells will be hard on the cache and waste memory.
		 * Setting count to ~10x the number of objects in the space is probably
		 * a good starting point. Tune from there if necessary.
		 *
		 * By default, dim is 100.0, and count is 1000.
		 *
		 * http://code.google.com/p/chipmunk-physics/wiki/cpSpace
		 * @param dim Size of the hash cells.
		 * @param count Suggested minimum number of cells in the hash table.
		 */
		void resizeActiveHash(cpFloat dim, int count);
		
		/**
		 * Rehashes the shapes in the static spatial hash. You only need to call
		 * this if you move a static shape.
		 *
		 * http://code.google.com/p/chipmunk-physics/wiki/cpSpace
		 */
		void rehashStatic();
		
		/**
		 * Step the simulation by the specified time step.
		 * @param dt The period to step the simulation by.
		 */
		virtual void step(cpFloat dt);
		
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
		 */
		virtual void sendObject();
		
	protected:
		cpSpace* _space;						/**< The Chipmunk space */
		
		BodyVector _bodyList;					/**< List of all bodies in the space */
		BodyVector _staticBodyList;				/**< List of all static bodies in the space */
		ShapeVector _staticShapeList;			/**< List of all static shapes in the space */
		ShapeVector _shapeList;					/**< List of all shapes in the space */
		JointVector _jointList;					/**< List of all joints in the space */
	};
}

#endif
