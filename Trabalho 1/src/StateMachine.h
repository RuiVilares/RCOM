#ifndef STATE_MACHINE
#define STATE_MACHINE

#include "FieldProcessor.h"
#include "Transmission.h"
#include <stdbool.h>
#include <unistd.h>

//state machine do llwrite enquanto está à espera de receber informação
bool stateMachineReadWrite(int fd, int timeout, char *buffer, int *size, char sequenceNumber);

//state machine do llread enquanto está à espera de receber informação
bool stateMachineRead(int *size, char *buffer, linkLayer *linkStruct);

//verifica se ocorreu algum erro
bool checkErrorOrSetOrDiscOrInfo(int *size, char *buffer, linkLayer *linkStruct);

//verifica a integridade da mensagem (dados) e faz antes destuffing
bool checkDataIntegity(int *size, char *buffer, linkLayer *linkStruct);

//faz destuffing da informação recebida
int destuffData(char *buffer, linkLayer *linkStruct);

//máquina de estados para o comando SET
bool stateMachineSet(int fd, int timeout);

//máquina de estados para a resposta UA
bool stateMachineUA(int fd, int timeout, int option);

//máquina de estados para o comando DISC
bool stateMachineDisc(int fd, int timeout);

#endif