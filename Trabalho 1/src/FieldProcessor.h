#ifndef FIELD_GENERATOR
#define FIELD_GENERATOR

#include "DefaultValues.h"
#include "GlobalVariable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//gera os primeiros 4 bytes das tramas (Flag, Endereço, Controlo, BCC1)
void generate4InitialBytesLLWR(char *field, int control);

//gera trama de rr
void generateInitialFromRR(char *field, char sequenceNumber);

//gera trama de rej
void generateInitialFromREJ(char *field, char sequenceNumber);

//verifica a integredidade da trama e responde adequadamente com RR ou REJ
bool checkIntegrityAndControlRRandREJ(char *buffer, int size, char sequenceNumber);

//obtem o campo de controlo para REJ do sequenceNumber
char getREJfromSN(char sequenceNumber);

//obtem o campo de controlo para RR do sequenceNumber
char getRRfromSN(char sequenceNumber);

#endif