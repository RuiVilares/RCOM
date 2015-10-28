#ifndef LINK_LAYER
#define LINK_LAYER

#include "DefaultValues.h"
#include "FieldProcessor.h"
#include "Transmission.h"
#include "StateMachine.h"
#include "GlobalVariable.h"
#include "LinkLayerStruct.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//cria uma trama de supervisão ou não numerada
//retorna true em caso de sucesso, retorna false em caso contrário
//field tem que ter 5 elementos
bool createSUField(char *field, int transmit, int typeAddress, int control);

//argumentos
//	– buffer: array de caracteres a transmitir
//	– length: comprimento do array de caracteres
//  – linkStruct: estrutura da link layer (é passada como argumento pela camada de aplicação,
//                mas apenas serve para evitar criar uma variável global)
//retorno
//	– número de caracteres escritos
//	– valor negativo em caso de erro
int llwrite(char * buffer, int length, linkLayer *linkStruct);

//faz stuffing ao pacote gerado pela aplicação
//length é o tamanho de data
//stuffedData é devolvido com stuffing + BCC(+1)
//o retorno é o tamanho de stuffedData
int stuffDataBCC(char *data, int length, char *stuffedData);

//argumentos
//	– buffer: array de caracteres recebidos
//  – linkStruct: estrutura da link layer (é passada como argumento pela camada de aplicação,
//                mas apenas serve para evitar criar uma variável global)
//retorno
//	– comprimento do array (número de caracteres lidos)
//	– valor negativo em caso de erro
int llread(char * buffer, linkLayer *linkStruct);

//função chamada por llread e llwrite para ler a informação recebida
//retorna true em caso de sucesso
//caso contrário retorna false
bool stateMachineReadWR(linkLayer *linkStruct);

//apesar de a trama ter chegado com erros
//tenta aproveitar alguma informação
//e responder ao emissor de acordo
void processDamagedDataI(linkLayer *linkStruct);

//verifica se o início da mensagem está imaculado
bool verifyIntegrityOfBeginning(char flag, char address, char control, char bcc);

//envia a resposta REJ
void sendREJ(int fd, char sequenceNumber);

//envia a resposta RR
void sendRR(int fd, char sequenceNumber);

//verifica se a mensagem recebida é DISC
bool checkDisc(char *frame, int frameSize);

//verifica se a mensagem recebida é SET
bool checkSet(linkLayer *linkStruct);

//argumentos
//	– porta: COM1, COM2, ...
//	– option: TRANSMITTER | RECEIVER
//  – linkStruct: estrutura da link layer (é passada como argumento pela camada de aplicação,
//                mas apenas serve para evitar criar uma variável global)
//retorno
//	– true -> ligação bem sucedida
//	– false -> houve algum erro (não conseguiu abrir a porta, não houve resposta do outro lado, ...)
bool llopen(char *portName, int option, linkLayer *linkStruct);

//argumentos
//	– option: TRANSMITTER | RECEIVER
//  – linkStruct: estrutura da link layer (é passada como argumento pela camada de aplicação,
//                mas apenas serve para evitar criar uma variável global)
//retorno
//	– valor positivo em caso de sucesso
//	– valor negativo em caso de erro
int llclose(linkLayer *linkStruct, int option);

//chamada de llclose caso seja transmitter
int closeTransmitter(linkLayer *linkStruct);

//chamada de llclose caso seja receiver
int closeReceiver(linkLayer *linkStruct);

#endif