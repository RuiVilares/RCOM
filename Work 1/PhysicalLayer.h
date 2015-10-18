//camada de nível físico

struct linkLayer {
char port[20]; /*Dispositivo /dev/ttySx, x = 0, 1*/
int baudRate; /*Velocidade de transmissão*/
unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
unsigned int timeout; /*Valor do temporizador: 1 s*/
unsigned int numTransmissions; /*Número de tentativas em caso de
falha*/
char frame[MAX_SIZE]; /*Trama*/
int maxFrameSize;
double errorProbability;
UserInterface userStruct;
}

//slide 18, 19, 20
