#ifndef APPLICATION_LAYER_STRUCT
#define APPLICATION_LAYER_STRUCT

#include "DefaultValues.h"
#include <stdbool.h>

typedef struct applicationLayerStruct {
	char port[20]; //Dispositivo /dev/ttySx, x = 0, 1
	int status; //TRANSMITTER | RECEIVER

	int fdFile;
	char fileName[128];
	int fileSize;
	int fileTransfer;

	char currentPacket[MAX_SIZE];
	int currentPacketSize;

	bool endFile;
	int sequenceNumber;
	int maxPacketSize; //subtrair 4 (inicio)
} applicationLayer;


#endif