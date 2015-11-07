#ifndef ALARM
#define ALARM

#include "GlobalVariable.h"
#include <signal.h>
#include <stdio.h>	

//função que é executada quando o alarme toca
void alarmHandler();

//instala o alarme
void setUpAlarmSignal();

#endif