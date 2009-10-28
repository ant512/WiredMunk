#ifndef _DEBUG_H_
#define _DEBUG_H_

#define DEBUG_ENABLED true

#include <stdio.h>

namespace WiredMunk {
	
	/**
	 * Provides a console-based method for printing debug information.  Can be
	 * disabled by setting DEBUG_ENABLED to false.
	 */
	class Debug {
	public:
		/**
		 * Output formatted values to the debug console.  Uses
		 * standard printf() syntax.
		 * @param format Format of the string to print.
		 * @param ... The values to output.
		 */
		static void printf(const char *format, ...);
	};
}

#endif
