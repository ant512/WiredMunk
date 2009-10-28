#ifndef _OPPOSING_BOXES_DEMO_H_
#define _OPPOSING_BOXES_DEMO_H_

#include "wiredmunkapp.h"
#include "body.h"
#include "space.h"
#include "shape.h"

namespace WiredMunk {
	
	class OpposingBoxesDemo : public WiredMunkApp {
	public:
		
		/**
		 * Constructor.
		 * @param serverIP IP address of the server.
		 * @param portNum Port number to connect to server on.
		 */
		OpposingBoxesDemo(const char* serverIP, int portNum);
		
		void keyboard(unsigned char key, int x, int y);
		
	private:
		Body* _body1;
		Body* _body2;
		int ticks;
		
		/**
		 * Startup code.
		 */
		void startup();
		
		/**
		 * Shutdown code.
		 */
		void shutdown();
		
		/**
		 * User code run whlst simulation in progress.
		 */
		void runUser();
	};
}

#endif
