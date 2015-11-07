#ifndef CLI
#define CLI

#include "ApplicationLayerStruct.h"
#include "LinkLayerStruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

//interface gráfica em que faz as diversas perguntas como baud rate ou timeout
void makeChoices(linkLayer *linkStruct);

//pergunta qual a baud rate que o utilizador pretende utilizador
//a baud rate tem de ser igual nas duas máquinas
//a baud rate já tem valores pré-definidos
void chooseBaudRate(linkLayer *linkStruct);

//escolhe o tamanho dos pacotes de dados
void chooseDataPacketsSize(applicationLayer *applicationStruct);

//escolhe o número máximo de retransmissões
void chooseRetransmissionsMax(linkLayer *linkStruct);

//escolhe o periodo de espera
void chooseTimeOut(linkLayer *linkStruct);

//inicia os valores iniciais das structs
void startValues(applicationLayer *applicationStruct, linkLayer *linkStruct);

#endif