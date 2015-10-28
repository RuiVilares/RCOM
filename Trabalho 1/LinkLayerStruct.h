#ifndef LINK_LAYER_STRUCT
#define LINK_LAYER_STRUCT

#include "DefaultValues.h"
#include <termios.h>
#include <unistd.h>

typedef struct linkLayerStruct {
	int fd; //Descritor correspondente à porta de série
	int baudRate; //Velocidade de transmissão
	char sequenceNumber; //Número de sequência da trama: no formato (0 ou 1)
	unsigned int timeout; //Valor do temporizador: 1 s
	unsigned int numTransmissions; //Número de tentativas em caso de falha
	char frame[FIELD_MAX_SIZE]; //Trama
	int frameSize;
	struct termios savetio;
} linkLayer;

#endif