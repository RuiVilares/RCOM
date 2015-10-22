#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BAUDRATE 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200

#DEFINE BAUDRATE B38400
#DEFINE MAX_SIZE 512
#DEFINE NUM_RETRIES = 3
#DEFINE TIMEOUT = 3


int main(int argc, char** argv) {
	if (argc != 1) {
		return 1;
	}

	if (argc == 1)
		startCLI();
	else if (argc == 5)
		procArgs(argc, argv);

	return 0;
}


