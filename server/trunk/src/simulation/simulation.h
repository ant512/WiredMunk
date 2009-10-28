#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <sys/time.h>

#include "space.h"
#include "socketeventhandler.h"
#include "positionsampler.h"

#define RESYNC_SECONDS 10

namespace WiredMunk {

	class Simulation : public SocketEventHandler {
	
	public:
		Simulation();
		~Simulation();
		
		/**
		 * Runs a single iteration of the simulation.  Should be called in a
		 * loop.
		 */
		virtual void run();
		
		/**
		 * Get a pointer to the space.
		 * @return A pointer to the space.
		 */
		inline Space* getSpace() { return _space; };
		
		/**
		 * Listens for incoming notifications about client object updates.
		 * @param msg Message data.
		 */
		void handleMessageReceived(const Message& msg);
		
		/**
		 * Receives serialised Chipmunk space from clients.  If no space
		 * definition is currently stored, this will create the space so that
		 * the server can begin its simulation.
		 */
		void handleSpaceReceived(const Message& msg);
		
		/**
		 * Receives serialised Chipmunk shape from clients.
		 */
		void handleShapeReceived(const Message& msg);
		
		/**
		 * Receives serialised Chipmunk body from clients.
		 */
		void handleBodyReceived(const Message& msg);
		
	private:
		Space* _space;
		struct timeval _lastRunTime;
		struct timeval _lastSyncTime;
		PositionSampler* _sampler;
		
		/**
		 * Steps the simulation.
		 */
		void step();
		
		/**
		 * Sync the simulation with clients if no communication has occurred
		 * within RESYNC_SECONDS.
		 */
		void sync();
	};
}

#endif
