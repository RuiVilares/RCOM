#ifndef TRANSMISSION
#define TRANSMISSION

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include "DefaultValues.h"
#include "LinkLayerStruct.h"
#include "GlobalVariable.h"

struct linkLayerStruct;

//retorna o file descriptor da porta aberta
int openPort(char *portName);

//configura a porta do descritor fd
//guarda as informações na oldtio
//linkStruct serve para ir buscar a baud rate
void setUpPort(linkLayer *linkStruct);

//fecha a porta no file descriptor
//e define a struct antiga
void closePort(int fd, struct termios oldtio);

//manda o buffer para o file descriptor fd
//tem em conta que se o buffer está cheio (espera WRITE_AGAIN_INTERVAL segundos)
//retorna o número de bytes escritos
int sendInformation(int fd, char *buffer, int length);

//ativa o alarme para alarmTime segundos
//quando o read retornar cancela todos os alarmes
//retorna o caracter lido em caso de sucesso
//retorna EXIT_ERROR em caso de erro e imprime uma mensagem de time out
char readByte(int fd, int alarmTime, char *readB);

//envia comando Set
void sendSet(int fd);

//envia mensagem UA com enddereço 0x03
void sendUA(int fd);

//envia mensagem UA com enddereço 0x01
void sendUA01(int fd);

//gera trama inicial para UA e SET com endereço 0x03
void generateFieldUAandSet(char *buffer);

//gera trama com comando DISC e envia
void sendDiscTransmitter(int fd);

//gera trama inicial para UA e SET com endereço 0x01
void generateFieldUAandSet01(char *buffer);

//envia o comando DISC da parte do RECEIVER
void sendDiscReceiver(int fd);

#endif