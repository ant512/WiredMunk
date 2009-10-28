#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>

#include "debug.h"

using namespace WiredMunk;

void Debug::printf(const char *format, ...) {
	
	if (DEBUG_ENABLED) {
		char buffer[256];
		
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);
		
		std::cout << buffer;
	}
}
