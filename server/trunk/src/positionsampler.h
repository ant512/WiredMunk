#ifndef _POSITION_SAMPLER_H_
#define _POSITION_SAMPLER_H_

#include <stdio.h>
#include "space.h"
#include "body.h"

#define SAMPLER_FILE_NAME "/Users/antonydzeryn/server_positions.txt"
#define SAMPLER_RESOLUTION 20
//#define SAMPLER_ENABLED

namespace WiredMunk {
	
	/**
	 * Takes a snapshot of the x, y and rotation values of all bodies in the
	 * space and saves them to a text file.  Values can be compared using the
	 * separate sampleranalyser program in order to determine how accurately
	 * WiredMunk stays in sync.
	 */
	class PositionSampler {
	public:
		PositionSampler() {
#ifdef SAMPLER_ENABLED
			_file = fopen(SAMPLER_FILE_NAME, "a+");
			_ticks = 0;
#endif
		}
		
		~PositionSampler() {
#ifdef SAMPLER_ENABLED
			fclose(_file);
#endif
		}
		
		void sample(Space* space) {
#ifdef SAMPLER_ENABLED
			_ticks++;
			
			if (_ticks == SAMPLER_RESOLUTION) {
				BodyVector* bodies = space->getBodies();
				Body* body;
				for (int i = 0; i < bodies->size(); ++i) {
					body = bodies->at(i);
					
					fprintf(_file, "%d,%f,%f,%f\n", body->getObjectId(), body->getPosition().x, body->getPosition().y, body->getAngle());
				}
				
				_ticks = 0;
			}
#endif
		}
			
	private:
		FILE* _file;
		int _ticks;
	};
}

#endif
