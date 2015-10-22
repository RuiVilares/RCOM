#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_


struct linkLayer {
	char port[20]; /*Dispositivo /dev/ttySx, x = 0, 1*/
	int baudRate; /*Velocidade de transmissão*/
	unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
	unsigned int timeout; /*Valor do temporizador: 1 s*/
	unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
	char frame[MAX_SIZE]; /*Trama*/
	int maxFrameSize;
	double errorProbability;
	UserInterface userStruct;
}

int llopen(int porta, TRANSMITTER | RECEIVER);

int llwrite(int fd, char * buffer, int length);

int llread(int fd, char * buffer);

int llclose(int fd);

//slide 18, 19, 20

#endif