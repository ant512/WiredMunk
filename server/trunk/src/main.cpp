#include "server.h"

#define DEFAULT_CLIENT_COUNT 2
#define DEFAULT_PORT_NUMBER 4444

using namespace WiredMunk;

int main (int argc, char * const argv[]) {
	
	int portNumber = DEFAULT_PORT_NUMBER;
	int clientCount = DEFAULT_CLIENT_COUNT;
	
	// Get settings from command line
	for (int i = 0; i < argc; ++i) {
		if (strncmp(argv[i], "-p", 2) == 0) {
			portNumber = atoi(argv[i + 1]);
		} else if (strncmp(argv[i], "-c", 2) == 0) {
			clientCount = atoi(argv[i + 1]);
		} else if (strncmp(argv[i], "-h", 2) == 0) {
			std::cout << "Usage: " << argv[0] << " [-c clients] [-p port]\n";
			return 0;
		}
	}

	Server server(clientCount, portNumber);
	server.run();
	
	return 0;
}
