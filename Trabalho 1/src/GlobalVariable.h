#ifndef GLOBAL_VARIABLE
#define GLOBAL_VARIABLE

#include "DefaultValues.h"
#include <stdbool.h>

//esta variável tem que ser global por causa do alarme
//para restringir o acesso e eventuais erros
//o acesso é feito exclusivamente pelas funções aqui declaradas
extern int numTransmissionsGlobal;

//guarda o valor inicial de numTransmissionsGlobal para quando for necessário fazer reset
extern int storedNumTransmissionsValueGlobal;

//quando o alarme tocar muda esta flag
extern bool alarmGlobal;

//estas variáveis servem para fornecer as estatisticas no final da transferência
extern int infoFieldGlobal;
extern int infoFieldReSentGlobal;
extern int timeOutNumberGlobal;
extern int rejNumGlobal;

//numTransmissionsGlobal passa a ter o valor de nTimes
void setUpGlobalTransmissionNum(int nTimes);

//diminui em 1 transmissão numTransmissionsGlobal
void decNumTransmissionsGlobal();

//obtem o número de numTransmissionsGlobal e
//retorna true caso numTransmissionsGlobal seja maior que zero
bool numTransmissionsGlobalGreaterThanZero();

//faz reset para o número original de numTransmissionsGlobal
void resetNumTransmissionsGlobal();

//torna a flag do alarme global igual a false
void resetAlarmGlobal();

//torna a flag do alarme global igual a true
void setAlarmGlobal();

#endif